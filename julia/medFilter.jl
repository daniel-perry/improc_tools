#
# The MIT License
#
# Copyright (c) 2013 University of Utah.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#

function indToLocation( datasize, index )
  # TODO: assuming 3D here, generalize to n-D:
  # ind = datasize[3]*datasize[2]*i1 + datasize[3]*i2 + i3
  # i3 "fastest index", last index is fastest in Julia..
  ind = index-1
  i1 = int64(floor(ind / (datasize[3]*datasize[2])))
  tmp = ind % (datasize[3]*datasize[2])
  i2 = int64(floor(tmp / datasize[3]))
  i3 = tmp % datasize[3]
  (i1+1,i2+1,i3+1)
end

function medFilter( data, radius, ind )
  datasize = size(data)
  loc = indToLocation(datasize, ind)
  # check if we're on the edge
  onedge = false
  for (i,sz) = zip(loc,datasize)
    if i<=radius || sz-i <= radius # on edge
      onedge = true
      break
    end
  end
  # compute median filter value if not on the edge.
  if onedge
    return 0 # return 0 for edge cases..
  else
    # TODO: we assume 3D, generlize this to n-D
    #regionSize = ntuple(length(loc), x -> 1+2*radius)
    #section = zeros( regionSize )

    section = data[ loc[1]-radius:loc[1]+radius, loc[2]-radius:loc[2]+radius, loc[3]-radius:loc[3]+radius ]
    return median(section)
  end
end

