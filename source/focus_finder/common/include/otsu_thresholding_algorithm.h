#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_OTSU_THRESHOLDING_ALGORITHM_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_OTSU_THRESHOLDING_ALGORITHM_H_

#include "thresholding_algorithm.h"
#include "image.h"

class OtsuThresholdingAlgorithmT : public ThresholdingAlgorithmT {
public:
  std::string getName() const;
  float calc(const ImageT & inImg, long bitDepth) const;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_OTSU_THRESHOLDING_ALGORITHM_H_*/
