/*
 The MIT License

 Copyright (c) 2011 University of Utah.

 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.

*/


/**
 * nrrd2silo - convert a vector volume from nrrd to silo (for use in visit)
 *  note: this seems to work, but runs extrememly slow - for a 64^3 volume of
 *        vectors it took around 20 minutes to convert... I've had more success
 *        with reading the nrrd directly into paraview, instead of using visit.
 *        Keeping this arond in case I ever have to deal with silo files again.
 */

#include <iostream>

// itk
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkNumericTraits.h>
#include <itkExceptionObject.h>

// silo
#include <silo.h>

int main(int argc, char ** argv)
{

  if( argc == 1 )
  {
    std::cerr << "usage: " << argv[0] << " input.nrrd output.silo" << std::endl;
    return 1;
  }
  std::string inputfn(argv[1]);
  std::string outputfn(argv[2]);

  // setup ITK types...
  typedef float  PixelType;
  typedef itk::Image< PixelType,  4 >   ImageType;
  typedef itk::ImageFileReader< ImageType  >  ReaderType;
  typedef ImageType::SizeType SizeType;
  typedef ImageType::IndexType IndexType;

  typedef itk::ImageRegionIteratorWithIndex< ImageType > ImageIterator;

  // read in the nrrds
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputfn );
  ImageType::Pointer input = reader->GetOutput();
  try
  {
    reader->Update();
  }
  catch(itk::ExceptionObject e)
  {
    std::cerr << "Error reading file " << inputfn << ": " << e << std::endl;
    return 1;
  }

  SizeType size = input->GetLargestPossibleRegion().GetSize();

  // setup silo to hold data:
  DBfile *file = DBCreate( outputfn.c_str(), DB_CLOBBER, DB_LOCAL, NULL, DB_PDB );

  size_t ndims = size.GetSizeDimension()-1; // last dimension (vector dimension) specified in DBPutQuadVar..
  if( size[ndims] != 3 )
  {
    std::cerr << "last dimensions must be 3." << std::endl;
    return 1;
  }
  int* dims = new int[ndims];
  float** coords = new float*[ndims];
  for(size_t i=0; i<ndims; ++i)
  {
    dims[i] = size[i];
    coords[i] = new float[dims[i]];
    for(size_t j=0; j<dims[i]; ++j)
    {
      coords[i][j] = j; // uniformly spaced grid 
    }
  }

  size_t loc = inputfn.find( '.' );
  while( loc != std::string::npos )
  {
    inputfn.replace(loc,1,1,'_');
    loc = inputfn.find( '.' );
  }

  // define the grid
  if(DBPutQuadmesh( 
    file, //dbfile
    inputfn.c_str(), //mesh name
    NULL, //coordnames
    coords, // array of pointers to coordinate arrays, size = ndims
    dims,   // array of dimensions, size = ndims
    ndims,  // number of dimensions
    DB_FLOAT, // silo datatype
    DB_COLLINEAR, // coordtype = DB_COLLINEAR (dim of coords param == 1) or DB_NONCOLLINEAR (dim of coords param == ndims)
    NULL // optlist - option list
  ))
  {
    std::cerr << "Failure calling DBPutQuadMesh while creating silo file." << std::endl;
    return 1;
  }

  char * varnames[] = {"x","y","z"};
  size_t nvars = 3;
  int var_ndims = 1; // 1 x 3 vectors ?
  int * var_dims = new int[var_ndims];
  for(size_t i=0; i<var_ndims; ++i)
  {
    var_dims[i] = 1;
  }

  ImageIterator it(input, input->GetLargestPossibleRegion() );

  size_t totalcount = 1;
  for(size_t i=0; i<size.GetSizeDimension()-1; ++i)
  {
    totalcount *= size[i];
  }
  std::cerr << "total count: " << totalcount << std::endl;

  size_t count = 0;
  // now add each of the vectors
  for(it.GoToBegin(); !it.IsAtEnd(); )
  {
    std::stringstream name;
    IndexType index = it.GetIndex();
    name << index[0] << '_' << index[1] << '_' << index[2];

    float ** vars = new float*[nvars];
    for(size_t i=0; i<nvars; ++i)
    {
      vars[i] = new float[1];
      vars[i][0] = it.Get(); 
      ++it; // increment it iterator
    }

    if(DBPutQuadvar( 
      file, //dbfile
      name.str().c_str(), //name of var
      inputfn.c_str(),    //name of associated mesh
      nvars, // nvar-D vector
      varnames, // names of individual vars
      vars, // array of length nvars containing ptrs to arrays of subvalues.
      var_dims, // array of length nvars containing length of subvalue arrays
      var_ndims, // number of dimensions
      NULL, // mixvars
      0, // mixlen
      DB_FLOAT, // silo datatype
      DB_NODECENT, // centering
      NULL // optlist
    ))
    {
      std::cerr << "Failure calling DBPutQuadVar while creating silo file." << std::endl;
      return 1;
    }
    if(count%10000==0)
    {
      std::cerr<<'.';
    }
    if( count > totalcount ) std::cerr << std::endl << std::endl << "over total count" << std::endl << std::endl;
    ++count;
  }

  // cleanup
  for(size_t i=0; i<ndims; ++i)
  {
    delete [] (coords[i]);
  }
  delete [] coords;
  delete [] dims;
  DBClose(file);

  return 0;
}

