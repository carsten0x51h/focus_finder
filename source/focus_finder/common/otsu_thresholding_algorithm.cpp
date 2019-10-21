#include "include/logging.h"
#include "include/otsu_thresholding_algorithm.h"
#include "include/floating_point_equality.h"

std::string OtsuThresholdingAlgorithmT::getName() const { return "OtsuThresholdingAlgorithmT"; }

float OtsuThresholdingAlgorithmT::calc(const ImageT & inImg, long bitDepth) const {

  LOG(debug) << "OtsuThresholdingAlgorithmT::calc..." << std::endl;

  size_t numBuckets = pow(2.0, bitDepth);
  std::vector<float> hist(numBuckets, 0.0F);
 
  float numPixels = inImg.width() * inImg.height();
  float sumB = 0.0F;
  float wB = 0.0F;
  float max = 0.0F;
  float threshold1 = 0.0F, threshold2 = 0.0F;

  LOG(debug) << "OtsuThresholdingAlgorithmT::calc - numBuckets: " << numBuckets << ", numPixels: " << numPixels << std::endl;

  // Calculate histogram - for some reason inImg.get_histogram() behaves different than expected.
  cimg_forXY(inImg, x, y) {
    int value = inImg(x,y);
    ++hist[value];
  }  
  
  float sum = 0;
  for (int pos = 0; pos < numBuckets; ++pos) {
    sum += pos * hist[pos];
  }
  

  for (int i = 0; i < numBuckets; ++i) {
    wB += hist[i];

    if (isAlmostEqual(wB, 0.0F)) { continue; }
 
    float wF = numPixels - wB;
    
    if (isAlmostEqual(wF, 0.0F)) { break; }
    
    sumB += i * hist[i];
 
    float mF = (sum - sumB) / wF;
    float mB = sumB / wB;
    float diff = mB - mF;
    float bw = wB * wF * pow(diff, 2.0);
    
    if (bw >= max) {
      threshold1 = i;
      if (bw > max) {
	threshold2 = i;
      }
      max = bw;            
    }
  } // end loop

  LOG(debug) << "OtsuThresholdingAlgorithmT::calc - threshold1: " << threshold1 << ", threshold2: " << threshold2 << std::endl;
  
  return (threshold1 + threshold2) / 2.0;
}
