#include <memory>

#include "include/focus_finder_factory.h"
#include "include/focus_finder_hyperbolic.h"


std::shared_ptr<FocusFinderT> FocusFinderFactoryT::getInstance(const FocusFinderStrategyT::TypeE & strategy) {

	// switch(strategy) {
	// case FocusFinderStrategyT::HYPERBOLIC:
	  // TODO: This is not correct - there is no "Hyperbolic" focus finder...
	  return std::make_shared<FocusFinderHyperbolicT>();

	// case FocusFinderStrategyT::LINEAR_SLOPE:
	// 	return nullptr; // TODO

	// default:
	// 	return nullptr;
	  //}
}
