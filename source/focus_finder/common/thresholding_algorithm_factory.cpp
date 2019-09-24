#include <memory>

#include "include/thresholding_algorithm_factory.h"
#include "include/otsu_thresholding_algorithm.h"
#include "include/max_entropy_thresholding_algorithm.h"

std::shared_ptr<ThresholdingAlgorithmT> ThresholdingAlgorithmFactoryT::getInstance(
		const ThresholdingAlgorithmTypeT::TypeE & type) {

	switch (type) {
	case ThresholdingAlgorithmTypeT::OTSU:
		return std::make_shared<OtsuThresholdingAlgorithmT>();
		
	case ThresholdingAlgorithmTypeT::MAX_ENTROPY:
		return std::make_shared<MaxEntropyThresholdingAlgorithmT>();

	default:
		return nullptr;
	}
}
