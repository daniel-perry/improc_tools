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




#include <iostream>
#include <sstream>


#ifdef WIN32
#include <windows.h>
#endif




#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionConstIterator.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageRegionIterator.h>



// setup ITK types...
typedef float PixelType;
typedef unsigned char MaskPixelType;
typedef itk::Image< PixelType,  3 >   ImageType;
typedef itk::Image< MaskPixelType,  3 >   MaskImageType;
typedef itk::ImageFileReader< ImageType  >  ReaderType;
typedef itk::ImageFileWriter< ImageType  >  WriterType;
typedef itk::ImageFileWriter< MaskImageType  >  MaskWriterType;
typedef itk::ImageRegionIterator< ImageType > IteratorType;
typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;


int main (int argc, char **argv) {

#ifdef WIN32
  ::SetErrorMode(0);
#endif


  try {

 
  if (argc != 3 && argc != 6) {
    std::cerr << "usage: " << argv[0] << " input.nrrd output.nrrd" << std::endl;
    return 1;
  }

  int arg_idx = 1;

  std::string data_filename = argv[arg_idx++];
  std::string mask_filename = argv[arg_idx++];

  // read data
  ReaderType::Pointer data_reader = ReaderType::New();
  data_reader->SetFileName(data_filename);
  ImageType::Pointer data_image = data_reader->GetOutput();
  data_reader->Update();

  // create a new image to store the mask
  MaskImageType::Pointer mask_image = MaskImageType::New(); 
  mask_image->SetRegions( data_image->GetLargestPossibleRegion() ); 
  mask_image->Allocate();
  mask_image->SetOrigin( data_image->GetOrigin() );
  mask_image->SetSpacing( data_image->GetSpacing() );
  mask_image->FillBuffer ( itk::NumericTraits<MaskImageType::PixelType>::Zero ); 

  // iterate through data image, creating mask image
  ConstIteratorType it(data_image, data_image->GetLargestPossibleRegion());
  it.GoToBegin();
  for( ; !it.IsAtEnd(); ++it) {
    PixelType pixel = it.Get();
    if (pixel > 0) {
      mask_image->SetPixel(it.GetIndex(), 1);
    }
  }

  // write trails image
  MaskWriterType::Pointer writer = MaskWriterType::New();
  writer->SetFileName(mask_filename);
  writer->SetInput(mask_image);
  writer->UseCompressionOn();
  writer->Update();

  return 0;

 } catch ( std::exception& e ) {
   std::cerr << "Exception caught: " << e.what() << std::endl;
   return -1;
 }
}



