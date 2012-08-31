/*
 The MIT License

 Copyright (c) 2012 University of Utah.

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

#ifndef ORIENTATIONVECTORFILTER_H
#define OREINTATIONVECTORFILTER_H

// itk
#include <itkImage.h> 
#include <itkSmartPointer.h>
#include <itkImageToImageFilter.h>
#include <itkGradientImageFilter.h> 
#include <itkCovariantVector.h>


/** OrientationVectorFilter
 *    Computer structure tensor for each component of the image.
 *    Output will be an image of direction vectors from the tensor.
 */
template< class TInputImage,
          class TOperatorValue = float,
          class TOutputValue = float,
          class TOutputImage = itk::Image< itk::CovariantVector< TOutputValue,
                                                                 TInputImage::ImageDimension >,
                                           TInputImage::ImageDimension > >
class OrientationVectorFilter: public itk::ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /** types */
  typedef TInputImage                       InputImageType;
  typedef typename InputImageType::Pointer  InputImagePointer;
  typedef TOperatorValue                OperatorValueType;
  typedef typename itk::Image<OperatorValueType,InputImageType::ImageDimension> OperatorImageType;
  typedef TOutputValue                  OutputValueType;
  typedef TOutputImage                  OutputImageType;
  typedef typename OutputImageType::PixelType OutputVectorType;
  typedef typename OutputImageType::Pointer OutputImagePointer;
  typedef typename itk::CovariantVector<OutputValueType,OutputImageType::ImageDimension> CovariantVectorType;
  typedef typename OutputImageType::RegionType OutputImageRegionType;

  typedef OrientationVectorFilter             Self;
  typedef typename itk::ImageToImageFilter<InputImageType,OutputImageType> Superclass;
  typedef typename itk::SmartPointer<Self>                                  Pointer;
  typedef typename itk::SmartPointer<const Self>                            ConstPointer;

  itkNewMacro(Self);

  itkTypeMacro(OrientationVectorFilter,  ImageToImageFilter);

  void SetSigma(float sigma){ m_Sigma=sigma; }
  float GetSigma(){ return m_Sigma; }
  void SetSigmaOuter(float sigmaOuter){ m_SigmaOuter=sigmaOuter; }
  float GetSigmaOuter(){ return m_SigmaOuter; }

  
protected:
  OrientationVectorFilter();
  virtual ~OrientationVectorFilter();
  void PrintSelf(std::ostream& os, itk::Indent indent) const;
  //virtual void GenerateData();
  virtual void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                                    itk::ThreadIdType threadId);

private:
  float m_Sigma;
  float m_SigmaOuter;

};

// definitions:
#include "OrientationVectorFilter.hxx"

#endif
