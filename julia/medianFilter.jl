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

# medianFilter.jl - an example of using the NrrdIO and Pmapi modules,
#                   to do a parallel median filter.

using NrrdIO
using Pmapi

function indToLocation( datasize, ind )
  # TODO: assuming 3D here, generalize to n-D:
  # ind = datasize[3]*datasize[2]*i1 + datasize[3]*i2 + i3
  # i3 "fastest index", last index is fastest in Julia..
  i1 = int64(floor(ind / (datasize[3]*datasize[2])))
  tmp = ind % (datasize[3]*datasize[2])
  i2 = int64(floor(tmp / datasize[3]))
  i1 = tmp % datasize[3]
  (i1,i2,i3)
end

function medFilter( data, radius, ind )
  datasize = size(data)
  loc = indToLocation(datasize, ind)
  onedge = false
  for (i,sz) = zip(loc,datasize)
    if i<=radius || sz-i <= radius # on edge
      onedge = true
      break
    end
  end
  if onedge
    return 0 # return 0 for edge cases..
  else
    # TODO: we assume 3D, generlize this to n-D
    #regionSize = ntuple(length(ind), x -> 1+2*radius)
    #section = zeros( regionSize )

    section = data[ ind[1]-radius:ind[1]+radius, ind[2]-radius:ind[2]+radius, ind[3]-radius:ind[3]+radius ]
    return median(section)
  end
end

function medianFilter( nrrd, radius )
  pmapi( x -> medFilter( nrrd.data, radius, x ), nrrd.data )
end

function main()
  if length(ARGS) != 3
    println("usage: scriptname <input.nrrd> <output.nrrd> <radius>")
    println("note: to use more than one thread, pass n > 1 to the\n-p argument when julia is invoked.")
    return
  end
  in_fn::String = ARGS[1]
  out_fn::String = ARGS[2]
  radius::Integer = parse_int(ARGS[3])

  in = readNrrd( in_fn )
  out = medianFilter( in, radius )
  writeNrrd( out_fn, out )
end

main()
