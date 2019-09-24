#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_THRESHOLDING_ALGORITHM_FACTORY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_THRESHOLDING_ALGORITHM_FACTORY_H_

#include <memory>

#include "thresholding_algorithm_type.h"
#include "enum_helper.h"

class ThresholdingAlgorithmT;

class ThresholdingAlgorithmFactoryT {
public:
	static std::shared_ptr<ThresholdingAlgorithmT> getInstance(const ThresholdingAlgorithmTypeT::TypeE & type);
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_THRESHOLDING_ALGORITHM_FACTORY_H_ */
