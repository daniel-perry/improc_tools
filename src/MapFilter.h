/*
 * Copyright (c) 2013 University of Utah
 */

#ifndef __MapFilter_H
#define __MapFilter_H

#include <itkImageToImageFilter.h>

template< 
          class TFunctor,
          class TInput,
          class TOutput
        >
class MapFilter : public itk::ImageToImageFilter<TInput,TOutput>
{
public:
  /** types */
  typedef TInput                  InType;
  typedef typename InType::Pointer InTypeP;
  typedef TOutput                 OutType;
  typedef typename OutType::Pointer OutTypeP;
  typedef typename OutType::RegionType OutRegion;
  typedef MapFilter           Self;
  typedef typename itk::ImageToImageFilter<InType,OutType> Superclass;
  typedef typename itk::SmartPointer<Self>                                  Pointer;
  typedef typename itk::SmartPointer<const Self>                            ConstPointer;
  typedef TFunctor FType;

  itkNewMacro(Self);
  itkTypeMacro(MeanFilter,  ImageToImageFilter);

  void SetFunctor(FType & f) { m_functor = f; }
  const FType & GetFunctor() const { return m_functor; }
  FType & GetFunctor() { return m_functor; }

protected:
  //virtual void BeforeThreadedGenerateData();
  //virtual void AllocateOutputs();
  virtual void ThreadedGenerateData(const OutRegion & outputRegionForThread,
                                    itk::ThreadIdType threadId);
  //virtual void AfterThreadedGenerateData();

private:
  FType m_functor;
 
};

#include "MapFilter.hxx" // implementation

#endif
