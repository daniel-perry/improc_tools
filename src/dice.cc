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





#include <vector>
#include <iostream>
#include <iomanip>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkNrrdImageIO.h>
#include <itkImageRegionConstIterator.h>


template < class ImageType >
double get_overlap(const typename ImageType::Pointer &first_image, 
                   const typename ImageType::Pointer &second_image) {


  typedef typename itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
  typedef typename ImageType::PixelType PixelType;


  typename ImageType::SizeType first_image_size = first_image->GetLargestPossibleRegion().GetSize();
  typename ImageType::SizeType second_image_size = second_image->GetLargestPossibleRegion().GetSize();

  if(first_image_size != second_image_size)
  {
    std::cerr << "Error: sizes (" << first_image_size << " ; " << second_image_size << ") do not match." << std::endl;
    exit(1);
  }

  //std::cerr << "sizes = " << first_image_size << ", " << second_image_size << "\n";

  ConstIteratorType it1( first_image, first_image->GetRequestedRegion() );
  ConstIteratorType it2( second_image, first_image->GetRequestedRegion() );
  it1.GoToBegin();
  it2.GoToBegin();

  long long num_pixels1 = 0;
  long long num_pixels2 = 0;
  long long num_overlap = 0;
  long long num_nooverlap = 0;

  while (!it1.IsAtEnd()) {
    PixelType pixel1 = it1.Get();
    PixelType pixel2 = it2.Get();

    if (pixel1 > 0) {
      num_pixels1++;
    }

    if (pixel2 > 0) {
      num_pixels2++;
    }

    if (pixel1 > 0 && pixel2 > 0) {
      num_overlap++;
    }

    if (pixel1 == 0 && pixel2 == 0) {
      num_nooverlap++;
    }

    ++it1;
    ++it2;
  }

  //std::cerr << num_pixels1 << ", " << num_pixels2 << ", " << num_overlap << "\n";
  long long tot_pixels = 1;
  for(size_t i=0; i<second_image_size.GetSizeDimension(); ++i) tot_pixels *= second_image_size[i];

  std::cerr << "tp: " << num_overlap << std::endl;
  std::cerr << "fp: " << num_pixels2-num_overlap << std::endl;
  std::cerr << "tn: " << num_nooverlap << std::endl;
  std::cerr << "fn: " << (tot_pixels-num_pixels2) - num_nooverlap << std::endl;
  std::cerr << "tot: " << tot_pixels << std::endl;

  double overlap = (2.0 * (double)num_overlap) / (double)(num_pixels1 + num_pixels2);

  return overlap * 100.0;
}



int main(int argc, char ** argv) {

  if (argc != 3) {
    std::cerr << "usage: " << argv[0] << " image1.nrrd image2.nrrd" << std::endl;
    return 1;
  }
  
  // setup ITK types...
  typedef float InputPixelType;
  typedef itk::Image< InputPixelType,  3 >   InputImageType;
  typedef itk::ImageFileReader< InputImageType  >  ReaderType;

  // read in the images...
  std::vector<InputImageType::Pointer> images;

  for( int i=1; i<argc; ++i ) {
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( argv[i] );
    InputImageType::Pointer image = reader->GetOutput();
    reader->Update();
    images.push_back(image);
  }


  double overlap = get_overlap<InputImageType>(images[0], images[1]);

  //std::cout << std::setprecision(4) << overlap << std::endl;
  std::cerr << "dice overlap: ";
  std::cout << std::setprecision(4) << overlap;
  std::cerr << std::endl;

  return 0;
}


