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
    std::cerr << "usage: " << argv[0] << " <in.nrrd> <compressed.nrrd>" << std::endl;
    return 1;
  }

  std::string inputfn(argv[1]);
  std::string outputfn(argv[2]);

  typedef itk::ImageIOBase::IOComponentType ScalarPixelType;
  itk::ImageIOBase::Pointer imageIO =
        itk::ImageIOFactory::CreateImageIO(
            inputfn.c_str(), itk::ImageIOFactory::ReadMode);
 
  imageIO->SetFileName(inputfn);
  imageIO->ReadImageInformation();
  const ScalarPixelType pixelType = imageIO->GetComponentType();
  //const size_t dimensions = imageIO->GetNumberOfDimensions();

  switch (pixelType)
  {
    case itk::ImageIOBase::UCHAR:
      writeImage<unsigned char,DIM>( outputfn, readImage<unsigned char,DIM>( inputfn ) );
      break;
    case itk::ImageIOBase::CHAR:
      writeImage<char,DIM>( outputfn, readImage<char,DIM>( inputfn ) );
      break;
    case itk::ImageIOBase::USHORT:
      writeImage<unsigned short,DIM>( outputfn, readImage<unsigned short,DIM>( inputfn ) );
      break;
    case itk::ImageIOBase::SHORT:
      writeImage<short,DIM>( outputfn, readImage<short,DIM>( inputfn ) );
      break;
    case itk::ImageIOBase::UINT:
      writeImage<unsigned int,DIM>( outputfn, readImage<unsigned int,DIM>( inputfn ) );
      break;
    case itk::ImageIOBase::INT:
      writeImage<int,DIM>( outputfn, readImage<int,DIM>( inputfn ) );
      break;
    case itk::ImageIOBase::ULONG:
      writeImage<unsigned long,DIM>( outputfn, readImage<unsigned long,DIM>( inputfn ) );
      break;
    case itk::ImageIOBase::LONG:
      writeImage<long,DIM>( outputfn, readImage<long,DIM>( inputfn ) );
      break;
    case itk::ImageIOBase::FLOAT:
      writeImage<float,DIM>( outputfn, readImage<float,DIM>( inputfn ) );
      break;
    case itk::ImageIOBase::DOUBLE:
      writeImage<double,DIM>( outputfn, readImage<double,DIM>( inputfn ) );
      break;

    default:
      std::cerr << "Pixel Type not supported. Exiting." << std::endl;
      return 1;
  }

  return 0;
}

