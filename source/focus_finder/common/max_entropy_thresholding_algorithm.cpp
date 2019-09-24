#include <cmath>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>

#include "include/image.h"
#include "include/logging.h"
#include "include/max_entropy_thresholding_algorithm.h"
#include "include/floating_point_equality.h"

std::string MaxEntropyThresholdingAlgorithmT::getName() const { return "MaxEntropyThresholdingAlgorithmT"; }

float MaxEntropyThresholdingAlgorithmT::calc(const ImageT & img, long bitDepth) const {

  LOG(debug) << "MaxEntropyThresholdingAlgorithmT::calc..." << std::endl;

  size_t numBins = 256;
  std::vector<float> hist(numBins, 0);

  // TODO: Use min_max()...
  float max;
  float min = img.min_max(max);

  LOG(debug) << "MaxEntropyThresholdingAlgorithmT::calc... bitDepth=" << bitDepth
	     << ", numBins=" << numBins << ", min=" << min << ", max=" << max
	     << ", img-dim (w x h)=" << img.width() << " x " << img.height() << std::endl;

  cimg_forXY(img, x, y) {
    int idx = (numBins - 1) * (img(x,y)-min) / (max - min);
    ++hist[idx];
  }

  
  // Normalize histogram (sum of all is 1)
  float sum = img.width() * img.height();
  
  std::vector<float> normHist(hist);
  for(std::vector<float>::iterator it = normHist.begin(); it != normHist.end(); ++it) {
    *it = *it / sum;
  }

  // Calculate accumulated histograms
  std::vector<float> accumulatedHistBlack(numBins, 0);
  std::vector<float> accumulatedHistWhite(numBins, 0);

  float accumHistSum = 0.0F;
  for (size_t idx = 0; idx < numBins; ++idx) {
    accumHistSum += normHist[idx];

    accumulatedHistBlack[idx] = accumHistSum;
    accumulatedHistWhite[idx] = 1.0F - accumHistSum; // TODO: not exactly 0 where it should be 0...
  }

  // Find first index of element not 0 in black distribution
  size_t first_bin_idx = 0;

  for (size_t idx = 0; idx < numBins; ++idx) {
     if ( ! isAlmostEqual(accumulatedHistBlack[idx], 0.0F) ) {
       first_bin_idx = idx;
       break;
     }
  }

  // Find last index of element not 0 in white distribution
  size_t last_bin_idx = numBins;

  for (size_t idx = numBins - 1; idx >= first_bin_idx; --idx) {
     if ( ! isAlmostEqual(accumulatedHistWhite[idx], 0.0F) ) {
       last_bin_idx = idx;
       break;
     }
  }

  LOG(debug) << "MaxEntropyThresholdingAlgorithmT::calc... first_bin_idx: " << first_bin_idx << ", last_bin_idx: " << last_bin_idx << std::endl;
  
  
  float threshold = 0;
  float max_ent = 0;
  float ent_back;
  float ent_obj;
  float tot_ent;
  
  for (size_t idx = first_bin_idx; idx < last_bin_idx; ++idx) {

    /* Entropy of the background pixels */
   ent_back = 0.0;
   
   for ( size_t ih = 0; ih <= idx; ih++ ) {
     if ( ! isAlmostEqual(normHist[ih], 0.0F) ) {
	float c = normHist[ih] / accumulatedHistBlack[idx];
	ent_back -= c * std::log(c);
      }
    }
   
   ent_obj = 0.0;

   for ( size_t ih = idx + 1; ih < numBins; ih++ ) {
     if ( ! isAlmostEqual(normHist[ih], 0.0F) ) {
	float c = normHist[ih] / accumulatedHistWhite[idx];
	ent_obj -= c * std::log(c);
      }
    }

   /* Total entropy */
   tot_ent = ent_back + ent_obj;

   if ( max_ent < tot_ent )
    {
     max_ent = tot_ent;
     threshold = idx;
    }
  }

  float th2 = min + (threshold / numBins) * (max - min);
  
  LOG(debug) << "MaxEntropyThresholdingAlgorithmT::calc...threshold: " << threshold << ", th2: " << th2 << std::endl;
 
  return th2;
}
