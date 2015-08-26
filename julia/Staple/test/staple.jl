# Daniel Perry
# Aug 2015

using Staple

im1 = rand(5,5,5)
im1[2:3,2:3,2:3] = 10
im2 = rand(5,5,5)
im2[3:4,2:3,3:4] = 10
im3 = rand(5,5,5)
im3[2:4,2:4,2:4] = 10

images = (im1,im2,im3)
beta = zeros(3)
lambda = ones(3)
maxiters = 100

result,final_beta,final_lambda = staple(images, beta, lambda, maxiters)

println("im1:")
println(im1)
println("im2:")
println(im2)
println("im3:")
println(im3)

println("")
println("")
println("result: ", size(result), ", ", maximum(result[:]), ", ", minimum(result[:]), ", ", mean(result[:]))
println(result)

println("")
println("")
println("beta: ", beta, " lambda:", lambda)
println("final_beta: ", final_beta, " final_lambda:", final_lambda)
