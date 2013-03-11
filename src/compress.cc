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
 * compress - compress a nrrd 
 */

#include <iostream>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkNumericTraits.h>
#include <itkExceptionObject.h>

#include "compress.h"

#define DIM 3

int main(int argc, char ** argv)
{

  if( argc == 1 )
  {
    std::cerr << "usage: " << argv[0] << " <in.nrrd> <compressed.nrrd> [uncompresss-flag]" << std::endl;
    return 1;
  }

  std::string inputfn(argv[1]);
  std::string outputfn(argv[2]);
  bool uncompress = false;
  bool vector = false;
  if(argc > 3)
  {
    uncompress = atoi(argv[3]) == 1;
  }

  typedef itk::ImageIOBase::IOComponentType ScalarPixelType;
  itk::ImageIOBase::Pointer imageIO =
        itk::ImageIOFactory::CreateImageIO(
            inputfn.c_str(), itk::ImageIOFactory::ReadMode);
 
  imageIO->SetFileName(inputfn);
  imageIO->ReadImageInformation();
  const ScalarPixelType pixelType = imageIO->GetComponentType();
  const size_t dimensions = imageIO->GetNumberOfDimensions();
  if( imageIO->GetPixelType() == itk::ImageIOBase::VECTOR || imageIO->GetPixelType() == itk::ImageIOBase::COVARIANTVECTOR )
    vector = true;

  switch (pixelType)
  {
    case itk::ImageIOBase::UCHAR:
      if(vector)
        writeImage< itk::VectorImage<unsigned char,DIM> >( outputfn, readImage< itk::VectorImage<unsigned char, DIM > >( inputfn ), uncompress );
      else
        writeImage< itk::Image<unsigned char,DIM> >( outputfn, readImage< itk::Image<unsigned char, DIM > >( inputfn ), uncompress );
      break;
    case itk::ImageIOBase::CHAR:
      if(vector)
        writeImage< itk::VectorImage<char,DIM> >( outputfn, readImage< itk::VectorImage<char, DIM > >( inputfn ), uncompress );
      else
        writeImage< itk::Image<char,DIM> >( outputfn, readImage< itk::Image<char, DIM > >( inputfn ), uncompress );
      break;
    case itk::ImageIOBase::USHORT:
      if(vector)
        writeImage< itk::VectorImage<unsigned short,DIM> >( outputfn, readImage< itk::VectorImage<unsigned short, DIM > >( inputfn ), uncompress );
      else
        writeImage< itk::Image<unsigned short,DIM> >( outputfn, readImage< itk::Image<unsigned short, DIM > >( inputfn ), uncompress );
      break;
    case itk::ImageIOBase::SHORT:
      if(vector)
        writeImage< itk::VectorImage<short,DIM> >( outputfn, readImage< itk::VectorImage<short, DIM > >( inputfn ), uncompress );
      else
        writeImage< itk::Image<short,DIM> >( outputfn, readImage< itk::Image<short, DIM > >( inputfn ), uncompress );
      break;
    case itk::ImageIOBase::UINT:
      if(vector)
        writeImage< itk::VectorImage<unsigned int,DIM> >( outputfn, readImage< itk::VectorImage<unsigned int, DIM > >( inputfn ), uncompress );
      else
        writeImage< itk::Image<unsigned int,DIM> >( outputfn, readImage< itk::Image<unsigned int, DIM > >( inputfn ), uncompress );
      break;
    case itk::ImageIOBase::INT:
      if(vector)
        writeImage< itk::VectorImage<int,DIM> >( outputfn, readImage< itk::VectorImage<int, DIM > >( inputfn ), uncompress );
      else
        writeImage< itk::Image<int,DIM> >( outputfn, readImage< itk::Image<int, DIM > >( inputfn ), uncompress );
      break;
    case itk::ImageIOBase::ULONG:
      if(vector)
        writeImage< itk::VectorImage<unsigned long,DIM> >( outputfn, readImage< itk::VectorImage<unsigned long, DIM > >( inputfn ), uncompress );
      else
        writeImage< itk::Image<unsigned long,DIM> >( outputfn, readImage< itk::Image<unsigned long, DIM > >( inputfn ), uncompress );
      break;
    case itk::ImageIOBase::LONG:
      if(vector)
        writeImage< itk::VectorImage<long,DIM> >( outputfn, readImage< itk::VectorImage<long, DIM > >( inputfn ), uncompress );
      else
        writeImage< itk::Image<long,DIM> >( outputfn, readImage< itk::Image<long, DIM > >( inputfn ), uncompress );
      break;
    case itk::ImageIOBase::FLOAT:
      if(vector)
        writeImage< itk::VectorImage<float,DIM> >( outputfn, readImage< itk::VectorImage<float, DIM > >( inputfn ), uncompress );
      else
        writeImage< itk::Image<float,DIM> >( outputfn, readImage< itk::Image<float, DIM > >( inputfn ), uncompress );
      break;
    case itk::ImageIOBase::DOUBLE:
      if(vector)
        writeImage< itk::VectorImage<double,DIM> >( outputfn, readImage< itk::VectorImage<double, DIM > >( inputfn ), uncompress );
      else
        writeImage< itk::Image<double,DIM> >( outputfn, readImage< itk::Image<double, DIM > >( inputfn ), uncompress );
      break;

    default:
      std::cerr << "Pixel Type not supported. Exiting." << std::endl;
      return 1;
  }

  return 0;
}

