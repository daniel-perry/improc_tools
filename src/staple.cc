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
 * staple - run the staple algorithm on a set of segmentations.
 */

#include <vector>
#include <iostream>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkNrrdImageIO.h>

#include "staple.h"

int main(int argc, char ** argv)
{

  if( argc < 3 )
  {
    std::cerr << "usage: " << argv[0] << " image1.nrrd image2.nrrd [image3.nrrd ...] mask.nrrd outimage.nrrd" << std::endl;
    return 1;
  }
  
  // setup ITK types...
  typedef   float  InputPixelType;
  typedef itk::Image< InputPixelType,  3 >   InputImageType;
  typedef itk::ImageFileReader< InputImageType  >  ReaderType;

  // read in the images...
  std::vector<InputImageType::Pointer> images;

  for( int i=1; i<argc-1; ++i )
  {
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( argv[i] );
    InputImageType::Pointer image = reader->GetOutput();
    reader->Update();
    images.push_back(image);
  }

  std::string outname( argv[argc-1] );

  // debug:
  /*
  for( size_t i=0; i<images.size(); ++i )
  {
    std::cout << i << ": (" << images[i]->GetLargestPossibleRegion().GetSize()[0]
              << ", " << images[i]->GetLargestPossibleRegion().GetSize()[1]
              << ", " << images[i]->GetLargestPossibleRegion().GetSize()[2]
              << ")" << std::endl;
  }
  */


  // run the staple algorithm with comparisons...
  runStaple<InputImageType>( images, outname ); 

  return 0;
}

