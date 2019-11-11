#include <memory>

#include "include/focus_finder_factory.h"
#include "include/focus_finder_fast_curve_lookup.h"

class FocusControllerT;

std::shared_ptr<FocusFinderT> FocusFinderFactoryT::getInstance(
							       const FocusFinderStrategyT::TypeE & strategy, std::shared_ptr<FocusControllerT> focusAnalyzer) {

	switch (strategy) {
	case FocusFinderStrategyT::FAST_CURVE_LOOKUP:
		return std::make_shared<FocusFinderFastCurveLookupT>(focusAnalyzer);

	default:
		return nullptr;
	}
}
