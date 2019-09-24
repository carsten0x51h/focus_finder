#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_THRESHOLDING_ALGORITHM_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_THRESHOLDING_ALGORITHM_H_

#include <string>

#include "image.h"

class ThresholdingAlgorithmT {
public:
  virtual ~ThresholdingAlgorithmT() {};
  virtual std::string getName() const = 0;
  virtual float calc(const ImageT & inImg, long bitDepth) const = 0;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_THRESHOLDING_ALGORITHM_H_*/


