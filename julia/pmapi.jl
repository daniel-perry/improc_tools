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

module Pmapi

export pmapi

# pmapi - parallel map with index
# same as normal pmap, but instead of passing the value
# of the list, pass the index.
# (this makes image processing of neighborhoods a little
#  easier)
# This code is modified from the original pmap definition in
# the Julia standard library.
# Args:
# f - function
# lst - list
# return - list of results of applying f() to each item in lst
function pmapi(f, lst)
    np = nprocs()
    n = length(lst)
    results = cell(n)
    i = 1
    # function to produce the next work item from the queue.
    # in this case it's just an index.
    next_idx() = (idx=i; i+=1; idx)
    @sync begin
        for p=1:np
            if p != myid() || np == 1
                @spawnat myid() begin
                    while true
                        idx = next_idx()
                        if idx > n
                            break
                        end
                        results[idx] = remote_call_fetch(p, f, idx)
                    end
                end
            end
        end
    end
    results
end

end # module

