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

@everywhere include("medFilter.jl")

using NrrdIO
using Pmapi

function medianFilter( nrrd, radius )
  data = pmapi( x -> medFilter( nrrd.data, radius, x ), nrrd.data )
  makeNrrd( nrrd.numeric_type, nrrd.sizes, data )
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
