#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_OTSU_THRESHOLDING_ALGORITHM_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_OTSU_THRESHOLDING_ALGORITHM_H_

#include "image.h"

/**
 * NOTE: Use myCImg.get_threshold(th); to get the thresholded image.

 */
class OtsuThresholdingAlgorithmT {

public:
  static float calc(const ImageT & inImg, long bitDepth);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_OTSU_THRESHOLDING_ALGORITHM_H_*/
