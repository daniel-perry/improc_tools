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

/**
 * XOR Overlap
 * For details see paper: Perry, D. et al. "Automatic classification of scar tissue...".
 *                        SPIE Medical Imaging: Computer Aided Diagnosis. Feb 2012.
 */

template < class ImageType >
double get_xor_overlap( 
                   const typename ImageType::Pointer &mask_image, 
                   const typename ImageType::Pointer &first_image, 
                   const typename ImageType::Pointer &second_image) {


  typedef typename itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
  typedef typename ImageType::PixelType PixelType;


  typename ImageType::SizeType mask_image_size = mask_image->GetLargestPossibleRegion().GetSize();
  typename ImageType::SizeType first_image_size = first_image->GetLargestPossibleRegion().GetSize();
  typename ImageType::SizeType second_image_size = second_image->GetLargestPossibleRegion().GetSize();

  //std::cerr << "sizes = " << first_image_size << ", " << second_image_size << "\n";

  ConstIteratorType it0( mask_image, first_image->GetRequestedRegion() );
  ConstIteratorType it1( first_image, first_image->GetRequestedRegion() );
  ConstIteratorType it2( second_image, first_image->GetRequestedRegion() );
  it0.GoToBegin();
  it1.GoToBegin();
  it2.GoToBegin();

  long long num_pixel1_xor_pixel2 = 0;
  long long num_mask = 0;

  while (!it1.IsAtEnd()) {
    PixelType pixel0 = it0.Get();
    PixelType pixel1 = it1.Get();
    PixelType pixel2 = it2.Get();

    if( pixel0 > 0 ) {
      ++num_mask;
      // pixel1 xor pixel2
      if ( (pixel1 > 0 && pixel2 == 0) || (pixel1 == 0 && pixel2 > 0) ) {
        ++num_pixel1_xor_pixel2;
      }
    }

    ++it0;
    ++it1;
    ++it2;
  }

  //std::cerr << num_pixels1 << ", " << num_pixels2 << ", " << num_overlap << "\n";

  //double xor_overlap = (2.0 * (double)num_overlap) / (double)(num_pixels1 + num_pixels2);
  double xor_overlap = static_cast<double>(num_mask-num_pixel1_xor_pixel2) / num_mask;

  return xor_overlap * 100.0;
}



int main(int argc, char ** argv) {

  if (argc != 4) {
    std::cerr << "usage: " << argv[0] << " mask.nrrd image1.nrrd image2.nrrd" << std::endl;
    std::cerr << "where mask is " << std::endl;
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


  double overlap = get_xor_overlap<InputImageType>(images[0], images[1], images[2]);

  //std::cout << std::setprecision(4) << overlap << std::endl;
  std::cout << std::setprecision(4) << overlap;

  return 0;
}


