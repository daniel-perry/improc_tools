/*========================================
 *
 * Copyright (c) 2012 University of Utah
 *
 *========================================*/

#ifndef STRUCTURETENSORFILTER_H
#define STRUCTURETENSORFILTER_H

// itk
#include <itkImage.h> 
#include <itkSmartPointer.h>
#include <itkImageToImageFilter.h>
#include <itkGradientImageFilter.h> 
#include <itkCovariantVector.h>



/** StructureTensorFilter
 *    Computer structure tensor for each component of the image.
 *    Output will be an image of tensor vectors.
 */
template< class TInputImage,
          class TOperatorValue = float,
          class TOutputValue = float,
          class TOutputImage = itk::Image< itk::CovariantVector< TOutputValue,
                                                                 TInputImage::ImageDimension >,
                                           TInputImage::ImageDimension > >
class StructureTensorFilter: public itk::ImageToImageFilter< TInputImage, TOutputImage >
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

  typedef StructureTensorFilter             Self;
  typedef typename itk::ImageToImageFilter<InputImageType,OutputImageType> Superclass;
  typedef typename itk::SmartPointer<Self>                                  Pointer;
  typedef typename itk::SmartPointer<const Self>                            ConstPointer;

  itkNewMacro(Self);

  itkTypeMacro(StructureTensorFilter,  ImageToImageFilter);

  void SetSigma(float sigma){ m_Sigma=sigma; }
  float GetSigma(){ return m_Sigma; }
  void SetSigmaOuter(float sigmaOuter){ m_SigmaOuter=sigmaOuter; }
  float GetSigmaOuter(){ return m_SigmaOuter; }

  
protected:
  StructureTensorFilter();
  virtual ~StructureTensorFilter();
  void PrintSelf(std::ostream& os, itk::Indent indent) const;
  //virtual void GenerateData();
  virtual void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                                    itk::ThreadIdType threadId);

private:
  float m_Sigma;
  float m_SigmaOuter;

};

// definitions:
#include "StructureTensorFilter.hxx"

#endif
