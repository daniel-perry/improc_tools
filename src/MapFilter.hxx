/*
 * Copyright (c) 2013 University of Utah
 */

#ifndef __MapFilter_HXX
#define __MapFilter_HXX

template< class TFunctor,
          class TInput,
          class TOutput
        >
void
MapFilter<TFunctor,TInput,TOutput>
::ThreadedGenerateData(const OutRegion & outRegion, itk::ThreadIdType threadId)
{
  typename InType::ConstPointer in = this->GetInput();
  OutTypeP out = this->GetOutput();
  m_functor( in, out, outRegion );
}


#endif
