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

#ifndef STRUCTURETENSORFILTER_HXX
#define STRUCTURETENSORFILTER_HXX

// itk
#include <itkGradientImageFilter.h>
//#include <itkDiscreteGaussianImageFilter.h> // doesn't work with image adaptors.
#include <itkSmoothingRecursiveGaussianImageFilter.h>
#include <itkNthElementImageAdaptor.h>

template< class TInputImage, class TOperatorValueType, class TOutputValueType , class TOutputImageType >
StructureTensorFilter< TInputImage, TOperatorValueType, TOutputValueType, TOutputImageType >
::StructureTensorFilter()
:m_Sigma(1.f),
m_SigmaOuter(1.f)
{
}

template< class TInputImage, class TOperatorValueType, class TOutputValueType , class TOutputImageType >
StructureTensorFilter< TInputImage, TOperatorValueType, TOutputValueType, TOutputImageType >
::~StructureTensorFilter()
{}

template< class TInputImage, class TOperatorValueType, class TOutputValueType , class TOutputImageType >
void 
StructureTensorFilter< TInputImage, TOperatorValueType, TOutputValueType, TOutputImageType >
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << "sigma: " << m_Sigma << ", outser sigma: " << m_SigmaOuter;
}

template< class TInputImage, class TOperatorValueType, class TOutputValueType , class TOutputImageType >
void
StructureTensorFilter< TInputImage, TOperatorValueType, TOutputValueType, TOutputImageType >
::ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                       itk::ThreadIdType threadId)
//template< class TInputImage, class TOperatorValueType, class TOutputValueType , class TOutputImageType >
//void
//StructureTensorFilter< TInputImage, TOperatorValueType, TOutputValueType, TOutputImageType >
//::GenerateData()
{
  /* types */
  const size_t dim = InputImageType::ImageDimension;
  const size_t matrixDim = dim * dim;
  //typedef typename itk::DiscreteGaussianImageFilter<InputImageType,OperatorImageType> PreFilterType;
  typedef typename itk::SmoothingRecursiveGaussianImageFilter<InputImageType,OperatorImageType> PreFilterType;
  typedef typename itk::CovariantVector<OperatorValueType,InputImageType::ImageDimension> OperatorVectorType;
  typedef typename itk::Image<OperatorVectorType,InputImageType::ImageDimension> OperatorVectorImageType;
  typedef typename itk::GradientImageFilter<OperatorImageType,OperatorValueType,OperatorValueType,OperatorVectorImageType> GradientFilterType;
  typedef typename itk::CovariantVector<OperatorValueType,matrixDim> MatrixType;
  typedef typename itk::Image< MatrixType, InputImageType::ImageDimension > MatrixImageType;
  typedef typename itk::NthElementImageAdaptor<MatrixImageType,OutputValueType> AdaptorType;
  //typedef typename itk::DiscreteGaussianImageFilter<AdaptorType,OperatorImageType> PostFilterType;
  typedef typename itk::SmoothingRecursiveGaussianImageFilter<AdaptorType,OperatorImageType> PostFilterType;
  typedef vnl_matrix_fixed<OperatorValueType,dim,dim> LiteMatrixType;
  typedef vnl_vector_fixed<OperatorValueType,dim> LiteVectorType;
  typedef itk::SymmetricEigenAnalysis<LiteMatrixType,LiteVectorType,LiteMatrixType> EigenAnalysisType;
  /* iterator types */
  typedef typename itk::ImageRegionIterator<MatrixImageType> MatrixIteratorType;
  typedef typename itk::ImageRegionIterator<OperatorVectorImageType> GradIteratorType;
  typedef typename itk::ImageRegionIterator<OutputImageType> OutIteratorType;
  typedef typename itk::ImageRegionIterator<OperatorImageType> OperatorImageIteratorType;
  typedef typename itk::ImageRegionIterator<AdaptorType> AdaptorIteratorType;


  // input/output setup
  typename InputImageType::ConstPointer inputImage( this->GetInput() );

  OutputImagePointer output( this->GetOutput() );

  // smooth the input with sigma
  typename PreFilterType::Pointer preFilter = PreFilterType::New();
  //preFilter->SetVariance( m_Sigma * m_Sigma );
  preFilter->SetSigma( m_Sigma );
  preFilter->SetInput( inputImage );

  // compute gradient vectors
  typename GradientFilterType::Pointer gradFilter = GradientFilterType::New();
  gradFilter->SetInput( preFilter->GetOutput() );
  gradFilter->Update(); // triggers preFilter and gradFilter.

  // compute outer product of gradient vectors to get tensor matrix:
  typename MatrixImageType::Pointer matrixImage = MatrixImageType::New();
  matrixImage->SetLargestPossibleRegion( inputImage->GetLargestPossibleRegion() );
  matrixImage->SetBufferedRegion( outputRegionForThread );
  matrixImage->SetRequestedRegion( outputRegionForThread );
  matrixImage->Allocate(); // 9x input image size (in 3D)

  MatrixIteratorType matIt( matrixImage, outputRegionForThread );
  GradIteratorType gradIt( gradFilter->GetOutput(), outputRegionForThread );
  
  for( matIt.GoToBegin(), gradIt.GoToBegin(); !(matIt.IsAtEnd() || gradIt.IsAtEnd()); ++matIt,++gradIt)
  {
    OperatorVectorType grad = gradIt.Get();
    MatrixType m;
    // outer product:
    // matrix indexing:
    // 123
    // 456
    // 789
    size_t mat_i = 0;
    for(size_t i=0; i<dim; ++i)
    {
      for(size_t j=0; j<dim; ++j)
      {
        m[mat_i++] = grad[i] * grad[i];
      }
    }
    matrixImage->SetPixel( matIt.GetIndex(), m );
    //matIt.Set( m );
  }

  // smooth each component of the tensor matrix:
  typename PostFilterType::Pointer postFilter = PostFilterType::New();
  //postFilter->SetVariance( m_SigmaOuter * m_SigmaOuter );
  postFilter->SetSigma( m_SigmaOuter );
  typename AdaptorType::Pointer adaptor = AdaptorType::New();
  adaptor->SetImage(matrixImage);
         
  for(size_t dim = 0; dim<matrixDim; ++dim)
  {
    adaptor->SelectNthElement(dim);
    postFilter->SetInput( adaptor );
    postFilter->Update();
    typename OperatorImageType::Pointer smoothedComponent = postFilter->GetOutput();

    // copy over smoothed values
    OperatorImageIteratorType smoothedIt(smoothedComponent, outputRegionForThread );
    AdaptorIteratorType adaptorIt(adaptor, outputRegionForThread );
    for( ; !(smoothedIt.IsAtEnd() || adaptorIt.IsAtEnd()); ++smoothedIt,++adaptorIt )
    {
      adaptorIt.Set( smoothedIt.Get() );
    }
  }

  // calculate eigenvectors of each matrix and store largest eigenvector in output:
  EigenAnalysisType eigenAnalysis;
  //eigenAnalysis.SetOrderEigenValues(true);
  eigenAnalysis.SetOrderEigenMagnitudes(true);
  eigenAnalysis.SetDimension(dim);

  OutIteratorType outIt( output, outputRegionForThread );
  LiteMatrixType tensorMat;
  LiteVectorType eigenVals;
  LiteMatrixType eigenVecs;

  // debug:
  //OutIteratorType dIt( gradFilter->GetOutput(), outputRegionForThread );

  for( matIt.GoToBegin(), outIt.GoToBegin(); !(matIt.IsAtEnd() || outIt.IsAtEnd()); ++matIt,++outIt)//,++dIt)
  {
    size_t mat_i = 0;
    for(size_t i=0; i<dim; ++i)
    {
      for(size_t j=0; j<dim; ++j)
      {
        tensorMat[i][j] = (matIt.Get())[mat_i++];
      }
    }

    eigenAnalysis.ComputeEigenValuesAndVectors( tensorMat, eigenVals, eigenVecs );

    OutputVectorType out;
    // eigenVecs: matrix of column vectors
    out[0] = eigenVecs[0][0];
    out[1] = eigenVecs[0][1];
    out[2] = eigenVecs[0][2];
    out /= out.GetNorm(); // normalize vector
    
    //output->SetPixel( outIt.GetIndex(), dIt.Get() );
    output->SetPixel( outIt.GetIndex(), out );
    //output.Set( out );
  }
}

#endif
