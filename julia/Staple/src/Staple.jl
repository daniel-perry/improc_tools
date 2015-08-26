# implementation of continuous staple as described in [1] below.
# [1] Olivier Commowick and Simon K. Warfield, "A Continuous STAPLE for Scalar, Vector, and Tensor Images: An Application to DTI Analysis", IEEE Trans on Med Imaging, 2009

using Distributions

module Staple

export staple

# this computes a new result and beta 
function e_step(result,images,beta,lambda)
	# see specifically equation 8 and 9 in the paper
	variance = 0
	for ii=1:length(images)
		variance += 1/lambda[ii]
	end
	variance = 1/variance

	new_beta = zeros(length(images))

	for i=1:size(result,1)
	  for j=1:size(result,2)
		  for k=1:size(result,3)
				mean = 0
				for ii=1:length(images)
					mu = images[ii][i,j,k] - beta[ii]
					mean += mu/lambda[ii]
				end
				mean *= variance
				result[i,j,k] = mean

				for ii=1:length(images)
					new_beta[ii] += images[ii][i,j,k] - mean
				end
			end
		end
	end
	
	new_beta /= length(result[:])

	return result, new_beta
end

# this computes a new lambda
function m_step(result,images,beta,lambda)
	variance = 0
	for ii=1:length(images)
		variance += 1/lambda[ii]
	end
	variance = 1/variance

	new_lambda = zeros(length(images))

	for i=1:size(result,1)
	  for j=1:size(result,2)
		  for k=1:size(result,3)
				mean = 0
				for ii=1:length(images)
					mu = images[ii][i,j,k] - beta[ii]
					mean += mu/lambda[ii]
				end
				mean *= variance

				for ii=1:length(images)
					gamma = mean + beta[ii] - images[ii][i,j,k]
					new_lambda[ii] += gamma^2
				end
			end
		end
	end

	new_lambda /= length(result[:])

	new_lambda .+= variance

	return new_lambda
end

# Staple algorithm for continuous images.
# NOTE: This implementation assumes a uniform prior as mentioned in [1]
#
# params:
#  images - list of images
#  init_beta - initial bias term for each image
#  init_lambda - initial variance term for each image
#  maxiters - max num iterations
#
function staple(images, init_beta, init_lambda, maxiters=100)

	imdim = length(size(images[1]))
	imsize = size(images[1])
	println("images: ", length(images))
	println("imdim: ", imdim)
	println("imsize: ", imsize)

	result = zeros(imsize)
	beta = copy(init_beta)
	lambda = copy(init_lambda)

	# run EM to compute the staple ground truth:
	iters = 1
	for iters=1:maxiters
		result,beta = e_step(result,images,beta,lambda)
		lambda = m_step(result,images,beta,lambda)
	end

	println("iters: ", iters)

	return result, beta, lambda
end

end # module
