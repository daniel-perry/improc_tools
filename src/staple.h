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
 * staple - run the staple algorithm on a set of segmentations.
 * this was copied from the Staple filter in corview, it should
 * produce the same results.
 */

#include <vector>

#include <itkSTAPLEImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageSliceConstIteratorWithIndex.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkImageFileWriter.h>

template < class ImageType >
void runStaple(const std::vector<typename ImageType::Pointer> & images, const std::string & outname, bool round_robin)
{
    size_t ignoreSlices = 0;
    // bounding box ImageType
    typedef typename ImageType::RegionType RegionType;
    typedef itk::ImageRegionIteratorWithIndex<ImageType> RegionIteratorType;
    std::vector<RegionIteratorType> iterators;
    for( unsigned int i=0; i<images.size(); ++i )
    {
      RegionIteratorType it( images[i], images[i]->GetLargestPossibleRegion() );
      iterators.push_back(it);
    }

    typename RegionType::SizeType  imgSize   = images[0]->GetLargestPossibleRegion().GetSize();
    typename RegionType::IndexType imgCorner = images[0]->GetLargestPossibleRegion().GetIndex();

    // Search for bounding box.
    const unsigned int PAD = 2;
    typename ImageType::IndexType idx;
    typename ImageType::IndexType lower = imgCorner + imgSize;
    typename ImageType::IndexType upper = imgCorner;

    typename ImageType::IndexType nonZeroIndex;

    while ( !iterators[0].IsAtEnd() )
    {
      bool nonZeroValue = false;
      for ( unsigned int i=0; i<iterators.size(); ++i )
      {
        if ( iterators[i].Get() != 0.f )
        {
          idx = iterators[i].GetIndex();
          nonZeroIndex = idx;
          nonZeroValue = true;
          break;
        }
      }

      if (nonZeroValue) 
      {
        for ( unsigned int i=0; i<idx.GetIndexDimension(); ++i)
        {
          if (lower[i] > idx[i]) 
          {
            lower[i] = static_cast<long unsigned int>(std::max<long signed int>( static_cast<long signed int>(idx[i] - PAD), 0 ));
          }
          if (upper[i] < idx[i]) 
          {
            upper[i] = static_cast<long unsigned int>(std::min<long signed int>( static_cast<long signed int>(idx[i] + PAD), imgSize[i]-1 ));
          }
        }
      }

      // advance all the iterators:
      for ( unsigned int i=0; i<iterators.size(); ++i )
      {
        ++(iterators[i]);
      }
    }

    typename RegionType::SizeType regionSize;
    for (unsigned int i = 0; i < regionSize.GetSizeDimension(); ++i)
    {
      regionSize[i] = upper[i] - lower[i] ;
    }

    RegionType regionOfInterest;
    regionOfInterest.SetSize(regionSize);
    regionOfInterest.SetIndex(lower);

    // Further reduce the region to exclude top and bottom N slices, as indicated by the filter.
    // By top N slices we mean the top and bottom N slices where all images have a value.
    size_t sliceIndex = 2;
    typedef itk::ImageSliceConstIteratorWithIndex< ImageType > SliceIteratorType;
    std::vector< SliceIteratorType > sliceIterators;
    for( size_t i=0; i<images.size(); ++i )
    {
      SliceIteratorType sliceIt( images[i], regionOfInterest );
      sliceIterators.push_back( sliceIt );
    }
    while( (ignoreSlices) > 0 && ! sliceIterators[0].IsAtEnd() )
    {
      std::vector<bool> nonZero(sliceIterators.size());
      for(size_t i=0; i<sliceIterators.size(); ++i) { nonZero[i] = false; }
      bool allNonZero = true;
      while( ! sliceIterators[0].IsAtEndOfSlice() )
      {
        while( ! sliceIterators[0].IsAtEndOfLine() )
        {
          allNonZero = true;
          for( size_t i=0; i<sliceIterators.size(); ++i )
          {
            // WEIRD: sliceIterators[i].Get() always returns 0... so we do the following
            // to get the actual value.
            typename SliceIteratorType::PixelType pix = images[i]->GetPixel( sliceIterators[i].GetIndex() ) ;
            if ( pix != 0.f )
            {
              nonZero[i] = true;
            }
            allNonZero = nonZero[i] && allNonZero;
            ++(sliceIterators[i]);
          }
          if ( allNonZero )
          {
            break;
          }
        }
        if ( allNonZero )
        {
          break;
        }
        // advance all the iterators
        for( size_t i=0; i<sliceIterators.size(); ++i )
        {
          sliceIterators[i].NextLine();
        }
      }
      if ( allNonZero )
      {
        // this is the first slice where all images have a value.
        // advance the iterator to find the Nth slice from here:
        for( size_t n=0; n<(ignoreSlices); ++n )
        {
          sliceIterators[0].NextSlice();
        }
        // the iterator now points at the first slice we want to include:
        typename SliceIteratorType::IndexType ind = sliceIterators[0].GetIndex();
        if( ind[sliceIndex] > lower[sliceIndex] )
        {
          lower[sliceIndex] = ind[sliceIndex];
        }
        break;
      }
      // advance all the iterators
      for( size_t i=0; i<sliceIterators.size(); ++i )
      {
        sliceIterators[i].NextSlice();
      }
    }
    // Now we do the same, but from the bottom:
    for( size_t i=0; i<sliceIterators.size(); ++i )
    {
      sliceIterators[i].GoToReverseBegin();
    }
    while( (ignoreSlices) > 0 && ! sliceIterators[0].IsAtReverseEnd() )
    {
      std::vector<bool> nonZero(sliceIterators.size());
      bool allNonZero = true;
      while( ! sliceIterators[0].IsAtReverseEndOfSlice() )
      {
        while( ! sliceIterators[0].IsAtReverseEndOfLine() )
        {
          allNonZero = true;
          for( size_t i=0; i<sliceIterators.size(); ++i )
          {
            typename SliceIteratorType::PixelType pix = images[i]->GetPixel( sliceIterators[i].GetIndex() ) ;
            if ( pix != 0 )
            {
              nonZero[i] = true;
            }
            allNonZero = nonZero[i] && allNonZero;
            //advance iterators
            --(sliceIterators[i]);
          }
          if ( allNonZero )
          {
            break;
          }
        }
        if ( allNonZero )
        {
          break;
        }
        // advance all the iterators
        for( size_t i=0; i<sliceIterators.size(); ++i )
        {
          sliceIterators[i].PreviousLine();
        }
      }
      if ( allNonZero )
      {
        // this is the first slice from the bottom where all images have a value.
        // advance the iterator to find the Nth slice from here:
        for( size_t n=0; n<(ignoreSlices); ++n )
        {
          sliceIterators[0].PreviousSlice();
        }
        // the iterator now points at the first slice we want to include:
        typename SliceIteratorType::IndexType ind = sliceIterators[0].GetIndex();
        if( ind[sliceIndex] < upper[sliceIndex] )
        {
          upper[sliceIndex] = ind[sliceIndex];
        }
        break;
      }
      // advance all the iterators
      for( size_t i=0; i<sliceIterators.size(); ++i )
      {
        sliceIterators[i].PreviousSlice();
      }
    }

    // Re-compute region of interest:
    for (unsigned int i = 0; i < regionSize.GetSizeDimension(); ++i)
    {
      regionSize[i] = upper[i] - lower[i];
    }
    regionOfInterest.SetSize(regionSize);
    regionOfInterest.SetIndex(lower);


    std::vector<typename ImageType::Pointer> cropped;

    typedef itk::RegionOfInterestImageFilter< ImageType,
      ImageType > RegionFilterType;
    //typedef itk::ExtractImageFilter< ImageType,
    //  ImageType > RegionFilterType;

    for ( unsigned int i=0; i<images.size(); ++i )
    {
      typename RegionFilterType::Pointer regionFilter = RegionFilterType::New();
      regionFilter->SetRegionOfInterest(regionOfInterest);
      //regionFilter->SetExtractionRegion(regionOfInterest);
      regionFilter->SetInput(images[i]);
      try
      {
        regionFilter->Update();
  		} 
  		catch ( ... ) 
  		{
        std::cerr << "There was an error running the Region Filter!!!" << std::endl;
/*  			if ( this->check_abort() )
  			{
  				this->report_error( "Filter was aborted." );
  				return;
  			}

  			this->report_error( "ITK filter failed to complete." );
        */
  			return;
  		}
//   		if ( this->check_abort() ) return;

      cropped.push_back( regionFilter->GetOutput() );
    }

    // remove the mask from the cropped list..
    typename ImageType::Pointer mask = cropped.back();
    cropped.pop_back();

    // Define the type of filter that we use.
		typedef itk::STAPLEImageFilter< 
			ImageType, ImageType > filter_type;
			
		// Create a new ITK filter instantiation.	
		typename filter_type::Pointer filter = filter_type::New();

		// Setup the filter parameters that we do not want to change.
    for ( size_t i = 0; i < cropped.size() ; ++i )
    {
  		filter->SetInput( i, cropped[i] );
    }

		// Run the actual ITK filter.
		// This needs to be in a try/catch statement as certain filters throw exceptions when they
		// are aborted. In that case we will relay a message to the status bar for information.
    double progress_value = 0.f;
    double progress_rate = 1.f/100.f;
		try 
		{ 
			filter->Update(); 
      //this->dst_layer_->update_progress(25, progress_value, progress_rate);
      progress_value += 25 * progress_rate;
		} 
		catch ( ... ) 
		{
      
			//if ( this->check_abort() )
			//{
		//		this->report_error( "Filter was aborted." );
		//		return;
		//	}

			//this->report_error( "ITK filter failed to complete." );
			std::cerr << "ITK filter failed to complete." << std::endl;
			return;
		}

		// As ITK filters generate an inconsistent abort behavior, we record our own abort flag
		// This one is set when the abort button is pressed and an abort is sent to ITK.
		//if ( this->check_abort() ) return;

    // copy the output of the filter into the full size output image
    typename ImageType::Pointer output = ImageType::New();
    output->SetBufferedRegion( images[0]->GetLargestPossibleRegion() );
    output->Allocate();
    output->FillBuffer( 0 );
    output->SetOrigin( images[0]->GetOrigin() );
    output->SetSpacing( images[0]->GetSpacing() );
    itk::ImageRegionIterator<ImageType> origOut( filter->GetOutput(), filter->GetOutput()->GetRequestedRegion() );
    itk::ImageRegionIterator<ImageType> newOut( output, regionOfInterest );
    for ( ; !origOut.IsAtEnd() && !newOut.IsAtEnd(); ++origOut, ++newOut )
    {
      newOut.Set( origOut.Get() );
    }

    typedef itk::ImageFileWriter< ImageType  >  WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetInput( output );
    writer->SetFileName( outname );
    writer->UseCompressionOn();
    writer->Update();

    // print overall results:
    std::cerr << "Overall Specificity:" << std::endl;
    for ( size_t i = 0; i<cropped.size() ; ++i )
    {
      std::cerr << i << ": " << filter->GetSpecificity(i) << std::endl;
      std::cout << "overallspecificity" << i << "," << filter->GetSpecificity(i) << std::endl;
    }
    std::cout << std::endl; 

    std::cerr << "Overall Sensitivity:" << std::endl;
    for ( size_t i = 0; i<cropped.size() ; ++i )
    {
      std::cerr << i << ": " << filter->GetSensitivity(i) << std::endl;
      std::cout << "overallsensitivity" << i << "," << filter->GetSensitivity(i) << std::endl;
    }
    std::cout << std::endl;

    if (!round_robin) return; // all done if no round robin comparison required...

    // now re-run in a round-robin fashion:

    for ( size_t excluded = 0; excluded < cropped.size(); ++excluded )
    {
      std::cerr << "excluding " << excluded << std::endl;
  		// Create a new ITK filter instantiation.	
	  	filter = filter_type::New();

	  	// Setup the filter parameters that we do not want to change.
      size_t inputNumber = 0;
      for ( size_t i = 0; i < cropped.size() ; ++i )
      {
        if ( i != excluded )
        {
    		  filter->SetInput( inputNumber++, cropped[i] );
        }
      }
	 	
	  	// Ensure we will have some threads left for doing something else
	  	//this->limit_number_of_itk_threads( filter );

	  	// Run the actual ITK filter.
	  	// This needs to be in a try/catch statement as certain filters throw exceptions when they
	  	// are aborted. In that case we will relay a message to the status bar for information.
    	try 
    	{ 
	  		filter->Update(); 
	  	} 
	  	catch ( ... ) 
	  	{
      /*
	  		if ( this->check_abort() )
	  		{
	  			this->report_error( "Filter was aborted." );
	  			return;
	  		}

	  		this->report_error( "ITK filter failed to complete." );
        */
	  		std::cerr <<  "ITK filter failed to complete." << std::endl;
	  		return;
	  	}

	  	// As ITK filters generate an inconsistent abort behavior, we record our own abort flag
	  	// This one is set when the abort button is pressed and an abort is sent to ITK.
	  	//if ( this->check_abort() ) return;

      // Update progress bar...
      //this->dst_layer_->update_progress(25, progress_value, progress_rate);
      progress_value += 25 * progress_rate;

      // Output specificity and sensitivity
      inputNumber = 0;
      std::cerr << "excluding image " << excluded << " Specificity, Sensitivity:" << std::endl;
      for ( size_t i = 0; i<cropped.size(); ++i )
      {
        if ( i != excluded )
        {
          std::cerr << i << ": " << filter->GetSpecificity(inputNumber) << ", " << filter->GetSensitivity(inputNumber) << std::endl;
          ++inputNumber;
        }
      }

      // Compare the excluded image with the resulting image at some threshold
      float threshold = .9;
      typedef itk::ImageRegionIterator<typename filter_type::InputImageType> in_iterator_type;
      typedef itk::ImageRegionIterator<typename filter_type::OutputImageType> out_iterator_type;
      in_iterator_type mask_it( mask, mask->GetRequestedRegion() );
      in_iterator_type candidate_it( cropped[excluded], cropped[excluded]->GetRequestedRegion() );
      out_iterator_type ground_truth_it( filter->GetOutput() , filter->GetOutput()->GetRequestedRegion() );
      std::vector<in_iterator_type> otherImages(cropped.size()-1);

      inputNumber = 0;
      for( size_t i=0; i<cropped.size() ; ++i )
      {
        if ( i != excluded )
        {
          otherImages[inputNumber++] = in_iterator_type( cropped[i], cropped[i]->GetRequestedRegion() );
        }
      }

      unsigned long TruePositive = 0;
      unsigned long FalsePositive = 0;
      unsigned long TrueNegative = 0;
      unsigned long FalseNegative = 0;
      unsigned long total = 0;
      //unsigned long overlap[3] = {0,0,0};
      //unsigned long overlapTotal[3] = {0,0,0};
      std::vector<unsigned long> overlap(cropped.size()-1);
      std::vector<unsigned long> overlapTotal(cropped.size()-1);
      unsigned long truthOverlap = 0;
      unsigned long truthOverlapTotal = 0;
      unsigned long truthOverlapDiff = 0;
      unsigned long maskSize = 0;
      for ( ; !candidate_it.IsAtEnd() && !ground_truth_it.IsAtEnd(); ++candidate_it, ++ground_truth_it, ++mask_it )
      {
        typename in_iterator_type::PixelType candidatePixel = candidate_it.Get();
        typename in_iterator_type::PixelType maskPixel = mask_it.Get();
        typename out_iterator_type::PixelType truthPixel = ground_truth_it.Get();

        bool candidate = (candidatePixel != static_cast<typename in_iterator_type::PixelType>(0));
        bool truth = (truthPixel >= threshold);

        ++total;
        if ( candidate && truth )
        {
          ++TruePositive;
        }
        else if ( candidate )
        {
          ++FalsePositive;
        }
        else if ( truth )
        {
          ++FalseNegative;
        }
        else
        {
          ++TrueNegative;
        }

        if ( candidate && truth )
        {
          ++truthOverlap;
          truthOverlapTotal += 2;
        }
        else if (candidate || truth )
        {
          ++truthOverlapTotal;
        }

        if( maskPixel > 0 )
        {
          ++maskSize;
          if( (candidate && !truth) || (!candidate && truth) )
          {
            ++truthOverlapDiff;
          }
        }

        // calculate overlap numbers
        for ( size_t i=0; i<cropped.size()-1; ++i )
        {
          bool other = ( (otherImages[i]).Get() != static_cast<typename in_iterator_type::PixelType>(0) );
          if ( other && candidate )
          {
            ++(overlap[i]);
            (overlapTotal[i])+=2;
          }
          else if ( other || candidate )
          {
            ++(overlapTotal[i]);
          }
          ++(otherImages[i]);
        }
      }

      std::cerr << "Stats (Ground Truth thresholded at " << threshold << "):" << std::endl
                << "True Positive: " << TruePositive << std::endl
                << "False Positive: " << FalsePositive << std::endl
                << "True Negative: " << TrueNegative << std::endl
                << "False Negative: " << FalseNegative << std::endl
                << "Total: " << total << std::endl;
      std::cerr << "Overlap:" << std::endl;
      inputNumber = 0;
      for (size_t i=0; i<cropped.size(); ++i)
      {
        if (i!=excluded)
        {
          std::cerr << i << ": " << (2*static_cast<double>(overlap[inputNumber])) / overlapTotal[inputNumber] << std::endl;
          ++inputNumber;
        }
      }
      // overlap with ground truth
      double truth = (2*static_cast<double>(truthOverlap)) / truthOverlapTotal;
      // overlap difference with ground truth
      double truthDiff = static_cast<double>(maskSize-truthOverlapDiff) / maskSize;
      std::cerr << "truth: " <<  truth << std::endl;
      std::cerr << "truth diff: " << truthDiff << std::endl;
      // To make it easier to pipe just numbers into files for compilation into a spreadsheet or db.
      std::cout << excluded << "truepositive," << TruePositive << std::endl
                << excluded <<  "falsepositive," << FalsePositive << std::endl
                << excluded <<   "truenegative," << TrueNegative << std::endl
                << excluded <<   "falsenegative," << FalseNegative << std::endl
                << excluded <<   "total," << total << std::endl << std::endl;
      inputNumber = 0;
      for (size_t i=0; i<cropped.size(); ++i)
      {
        if (i!=excluded)
        {
          std::cout << excluded <<   "overlap" << i << "," << (2*static_cast<double>(overlap[inputNumber])) / overlapTotal[inputNumber] << std::endl;
          ++inputNumber;
        }
      }
      std::cout << excluded << "truth," << truth << std::endl;
      std::cout << excluded << "truthdiff," << truthDiff << std::endl;
      std::cout << std::endl; // space between candidate output

    }

}


