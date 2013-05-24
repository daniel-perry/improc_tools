/*
 * Copyright (c) 2013 University of Utah
 */

// simple test of map.h utility.

// itk
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionConstIteratorWithIndex.h>

// local
#include "map.h"

template<class TIn,class TOut>
struct MapFunctor
{

  MapFunctor()
  {}

  typedef typename TIn::ConstPointer TInP;
  typedef typename TOut::Pointer TOutP;
  typedef typename TOut::RegionType OutRegion;
  typedef typename TIn::PixelType TPix;
  void operator()(const TInP & in, TOutP & out, const OutRegion & threadRegion)
  {
    // trivial operation: set all pixels to be 1d mahal distance from mean.
    typedef itk::ImageRegionConstIterator<TIn> It;
    It it(in,threadRegion);
    TPix mean = 0;
    size_t cnt = 0;
    for(it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      mean += it.Get();
      ++cnt;
    }
    mean /= cnt;
    TPix var = 0;
    for(it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      TPix diff = it.Get()-mean;
      var += diff*diff;
    }
    var /= (cnt-1);
    var = 1/var;
    for(it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      TPix diff = it.Get()-mean;
      TPix dist = std::sqrt( diff*diff*var ); // 1d mahalanobis?
      out->SetPixel(it.GetIndex(),dist);
    }
  }
};

// example functor for use with reduce<>::run()
template<class TIn,class TOut>
struct ReduceFunctor
{

  ReduceFunctor()
  {}

  typedef typename TIn::ConstPointer TInP;
  typedef typename std::vector<TOut> OutList;
  typedef typename TIn::RegionType InRegion;
  typedef typename TIn::PixelType TPix;
  TOut operator()(const TInP & in, const InRegion & threadRegion)
  {
    // trivial operation: compute the mean
    typedef itk::ImageRegionConstIterator<TIn> It;
    It it(in,threadRegion);
    TOut mean;
    mean.first = 0;
    mean.second = 0;
    for(it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      mean.first += it.Get();
      ++mean.second;
    }
    return mean;
  }
  TOut operator()(const OutList & in)
  {
    // trivial operation: compute the mean
    typename OutList::const_iterator it;
    TOut mean;
    mean.first = 0;
    mean.second = 0;
    for(it=in.begin(); it != in.end(); ++it)
    {
      std::cerr << it->first << ", " << it->second << std::endl;
      mean.first += it->first;
      mean.second += it->second;
    }
    std::cerr << "before div: " << mean.first << ", " << mean.second << std::endl;
    mean.first /= mean.second;
    mean.second = 0;
    return mean;
  }
};

int main(int argc, char * argv[])
{
  if(argc == 1)
  {
    std::cerr << "usage: " << argv[0] << " val in.nrrd out.nrrd" << std::endl;
    return 1;
  }
  float val = atof(argv[1]);
  std::string infn(argv[2]);
  std::string outfn(argv[3]);

  typedef float PType;
  const size_t dim = 3;
  typedef itk::Image<PType,dim> IType;

  typedef itk::ImageFileReader<IType> RType;
  RType::Pointer reader = RType::New();
  reader->SetFileName(infn);
  reader->Update();
  IType::ConstPointer in = reader->GetOutput();

  typedef MapFunctor<IType,IType> FType;
  FType functor;
  IType::Pointer out;
  std::cerr << "Running Map test... " << std::endl;
  std::cerr << "running single threaded... " << std::endl;
  map<IType,IType,FType>::run( in, out, functor , 1);
  std::cerr << "running with 6 threads... " << std::endl;
  map<IType,IType,FType>::run( in, out, functor , 6);

  typedef itk::ImageFileWriter<IType> WType;
  WType::Pointer writer = WType::New();
  writer->SetFileName(outfn);
  writer->SetInput(out);
  writer->Update();

  typedef std::pair<double,size_t> ObjT;
  typedef ReduceFunctor<IType,ObjT> RFType;
  RFType rfunctor;
  std::cerr << "Running Reduce test... " << std::endl;
  std::cerr << "running single threaded... " << std::endl;
  ObjT result = reduce<IType,ObjT,RFType>::run( in, rfunctor , 1);
  std::cerr << "result = " << result.first << std::endl;
  std::cerr << "running with 6 threads... " << std::endl;
  result = reduce<IType,ObjT,RFType>::run( in, rfunctor , 6);
  std::cerr << "result = " << result.first << std::endl;
  result = reduce<IType,ObjT,RFType>::run( in, rfunctor , 6);
  std::cerr << "result = " << result.first << std::endl;
  result = reduce<IType,ObjT,RFType>::run( in, rfunctor , 6);
  std::cerr << "result = " << result.first << std::endl;

  return 0;
}
