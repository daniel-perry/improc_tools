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
 * threshold - threshold the image to a specific value
 */

#include <iostream>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkNumericTraits.h>
#include <itkExceptionObject.h>


enum Operation
{
  ABOVE,
  BELOW
};

/**
 * thresholds an image creating a binary mask
 */
int main(int argc, char ** argv)
{

  if( argc == 1 )
  {
    std::cerr << "usage: " << argv[0] << " <above|below> <threshold-value> input.nrrd output.nrrd" << std::endl;
    return 1;
  }
  std::string operationstr(argv[1]);
  float threshold = atof(argv[2]);
  std::string inputfn(argv[3]);
  std::string outputfn(argv[4]);

  Operation operation(ABOVE);

  if(operationstr == "above")
  {
    operation = ABOVE;
  }
  else if(operationstr == "below")
  {
    operation = BELOW;
  }
 else
  {
    std::cerr << "Error: unrecognized logical operator " << operation << std::endl;
    return 1;
  }

  // setup ITK types...
  typedef float  PixelType;
  typedef itk::Image< PixelType,  3 >   ImageType;
  typedef itk::ImageFileReader< ImageType  >  ReaderType;

  typedef char  OutPixelType;
  typedef itk::Image< OutPixelType,  3 >   OutImageType;

  typedef itk::ImageRegionIteratorWithIndex< ImageType > ImageIterator;
  typedef itk::ImageRegionIteratorWithIndex< OutImageType > OutImageIterator;

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

  // store the results in a nrrd: 
  OutImageType::Pointer output = OutImageType::New(); 
  output->SetRegions( input->GetLargestPossibleRegion() ); 
  output->Allocate();
  output->SetOrigin( input->GetOrigin() );
  output->SetSpacing( input->GetSpacing() );
  //output->FillBuffer ( itk::NumericTraits<OutImageType::PixelType>::Zero ); 

  OutImageIterator outputIt( output, output->GetLargestPossibleRegion() );
  ImageIterator inputIt( input, input->GetLargestPossibleRegion() );

  for( ; !outputIt.IsAtEnd() ; ++outputIt, ++inputIt )
  {
    switch(operation)
    {
      case ABOVE:
        outputIt.Set( inputIt.Get() > threshold );
        break;
      case BELOW:
        outputIt.Set( inputIt.Get() < threshold );
        break;
    }
  }

  // write out the mask:
  typedef itk::ImageFileWriter< OutImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( output );
  writer->SetFileName( outputfn );
  writer->UseCompressionOn();

  try
  {
    writer->Update();
  }
  catch(itk::ExceptionObject e)
  {
    std::cerr << "Error writing file " << outputfn << ": " << e << std::endl;
  }

  return 0;
}

