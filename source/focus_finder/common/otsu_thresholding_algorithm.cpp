#include "include/otsu_thresholding_algorithm.h"

float OtsuThresholdingAlgorithmT::calc(const ImageT & inImg, long bitDepth) {

  auto hist = inImg.get_histogram(pow(2.0, bitDepth));
 
  float sum = 0;
  cimg_forX(hist, pos) { sum += pos * hist[pos]; }
 
  float numPixels = inImg.width() * inImg.height();
  float sumB = 0, wB = 0, max = 0.0;
  float threshold1 = 0.0, threshold2 = 0.0;
  
  cimg_forX(hist, i) { 
    wB += hist[i];
 
    if (wB != 0) { continue; }    
 
    float wF = numPixels - wB;
    
    if (wF != 0) { break; }
    
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
  
  float th = (threshold1 + threshold2) / 2.0;

  return th;
}
