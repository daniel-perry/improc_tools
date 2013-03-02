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
 * logical - logically combine two mask volumes
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
  AND,
  OR,
  XOR,
  NOT
};

/**
 * thresholds an image creating a binary mask
 */
int main(int argc, char ** argv)
{

  if( argc == 1 )
  {
    std::cerr << "usage: " << argv[0] << " <and|or|xor|not> <in1.nrrd> [in2.nrrd] <output.nrrd>" << std::endl;
    std::cerr << "note: when not is used, in2.nrrd should not be given." << std::endl;
    return 1;
  }
  std::string operationstr(argv[1]);
  std::string input1fn(argv[2]);
  std::string input2fn;
  std::string outputfn(argv[3]);

  Operation operation(NOT);

  if(operationstr == "and")
  {
    operation = AND;
  }
  else if(operationstr == "or")
  {
    operation = OR;
  }
  else if(operationstr == "xor")
  {
    operation = XOR;
  }
  else if(operationstr == "no")
  {
    operation = NOT;
  }
  else
  {
    std::cerr << "Error: unrecognized logical operator " << operation << std::endl;
    return 1;
  }

  if(operation != NOT)
  {
    input2fn = outputfn;
    outputfn = argv[4];
  }

  // setup ITK types...
  typedef unsigned char PixelType;
  typedef itk::Image< PixelType,  3 >   ImageType;
  typedef itk::ImageFileReader< ImageType  >  ReaderType;

  typedef unsigned char  OutPixelType;
  typedef itk::Image< OutPixelType,  3 >   OutImageType;

  typedef itk::ImageRegionIteratorWithIndex< ImageType > ImageIterator;
  typedef itk::ImageRegionIteratorWithIndex< OutImageType > OutImageIterator;

  // read in the nrrds
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( input1fn );
  ImageType::Pointer input1 = reader->GetOutput();
  try
  {
    reader->Update();
  }
  catch(itk::ExceptionObject e)
  {
    std::cerr << "Error reading file " << input1fn << ": " << e << std::endl;
    return 1;
  }

  ImageType::Pointer input2;
  if(!input2fn.empty())
  {
    reader = ReaderType::New();
    reader->SetFileName( input2fn );
    ImageType::Pointer input2 = reader->GetOutput();
    try
    {
      reader->Update();
    }
    catch(itk::ExceptionObject e)
    {
      std::cerr << "Error reading file " << input2fn << ": " << e << std::endl;
      return 1;
    }
  }


  // store the results in a nrrd: 
  OutImageType::Pointer output = OutImageType::New(); 
  output->SetRegions( input1->GetLargestPossibleRegion() ); 
  output->Allocate();
  output->SetOrigin( input1->GetOrigin() );
  output->SetSpacing( input1->GetSpacing() );
  //output->FillBuffer ( itk::NumericTraits<OutImageType::PixelType>::Zero ); 

  OutImageIterator outputIt( output, output->GetLargestPossibleRegion() );
  ImageIterator input1It( input1, input1->GetLargestPossibleRegion() );
  ImageIterator * input2It;
  if(input2.IsNotNull())
  {
    input2It = new ImageIterator( input2, input2->GetLargestPossibleRegion() );
  }

  for( ; !outputIt.IsAtEnd() ; ++outputIt, ++input1It )
  {
    switch(operation)
    {
      case AND:
        outputIt.Set( input1It.Get() && input2It->Get() );
        break;
      case OR:
        outputIt.Set( input1It.Get() || input2It->Get() );
        break;
      case XOR:
        outputIt.Set( !( input1It.Get() && input2It->Get()) && (input1It.Get() || input2It->Get()) );
        break;
      case NOT:
        outputIt.Set( !input1It.Get() );
        break;
    }
    if(input2It) ++(*input2It);
  }
  if(input2It) delete input2It;

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

