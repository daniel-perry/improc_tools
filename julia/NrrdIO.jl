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

module NrrdIO

# Very simple/limited nrrd io for Julia language...

export Nrrd, readNrrd, writeNrrd, makeNrrd

type Nrrd
  numeric_type
  dimension
  sizes
  kinds
  origin
  direction
  data
end

# read a nrrd in from a file.
function readNrrd(fn::String)
  f = open( fn )
  volume = Nrrd( typeof(1.0),
                 3,
                 [0,0,0],
                 "",
                 "",
                 "",
                 "")
  nrrdVersion = readline(f)
  while true
    line = chomp(readline(f))
    #println(line)
    if length(line) == 0
      break
    end
    if line[1] == '#' # comment, skip this line
      continue
    end
    parts = split(line,":")
    key = strip(parts[1])
    val = strip(parts[2])
    #println( "key: ", key, " val: ", val)
    if key == "type"
      if val == "double"
        volume.numeric_type = Float64
      elseif val == "float"
        volume.numeric_type = Float32
      elseif val == "unsigned char"
        volume.numeric_type = Uint8
      end
    elseif key == "dimension"
      volume.dimension = parse_int( val )
    elseif key == "sizes"
      parts = split(val," ")
      sizes = Array(typeof(volume.dimension), volume.dimension)
      for i in 1:volume.dimension
        sizes[i] = parse_int(parts[i])
      end
      volume.sizes = ntuple( length(sizes), x -> sizes[x] )
    elseif key == "encoding"
      if val != "raw"
        throw("Can't handle $(val) encoding, must be raw!")
      end
    elseif key == "endian"
      if val != "little"
        throw("Can't handle $(val) endianness, must be little endian!")
      end
    elseif key == "kinds"
      volume.kinds = val
    elseif key == "space directions"
      volume.direction = val
    elseif key == "space origin"
      volume.origin = val
    end
  end
  volume.data = read(f, volume.numeric_type, volume.sizes)
  close( f )
  #println("")
  #println(volume.numeric_type)
  #println(volume.dimension)
  #println(volume.sizes)
  #println(size(volume.data))
  #println("")
  return volume
end

# make and allocate a nrrd
function makeNrrd(numeric_type, sizes)
  volume = Nrrd( numeric_type,
                 length(sizes),
                 sizes,
                 "",
                 "",
                 "",
                 "")

  #TODO: make this part more general - this works for the few nrrd types I use...
  if volume.dimension > 3
    volume.kinds = "vector domain domain domain"
    volume.direction = "none (1,0,0) (0,1,0) (0,0,1)"
  elseif volume.dimension == 3
    volume.kinds = "domain domain domain"
    volume.direction = "(1,0,0) (0,1,0) (0,0,1)"
  end
  volume.origin = "(0,0,0)"
  volume.data = Array(volume.numeric_type, volume.sizes) # allocate the space
  return volume
end

# write a nrrd out to a file.
function writeNrrd(fn::String, volume::Nrrd)
  f = open( fn, false, true, true, false, false) # write mode
  write(f, "NRRD0004\n")
  write(f, "# Complete NRRD file format specification at:\n")
  write(f, "# http://teem.sourceforge.net/nrrd/format.html\n")
  if volume.numeric_type == Float64
    write(f, "type: double\n" )
  elseif volume.numeric_type == Float32
    write(f, "type: float\n" )
  elseif volume.numeric_type == Uint8
    write(f, "type: unsigned char\n" )
  end
  write(f, "dimension: $(volume.dimension)\n")
  sizestr = join(volume.sizes, " ")
  write(f, "sizes: $(sizestr)\n")
  if length(volume.direction) > 2
    write(f, "space directions: $(volume.direction)\n")
  end
  write(f, "kinds: $(volume.kinds)\n")
  write(f, "endian: little\n")
  write(f, "encoding: raw\n")
  if length(volume.origin) > 2
    write(f, "space origin: $(volume.origin)\n")
  end
  write(f, "\n") # header/data separator
  write(f, volume.data)
  close( f )
end

end # end module
