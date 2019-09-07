/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_FACTORY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_FACTORY_H_

#include <memory>

#include "enum_helper.h"

class FocusFinderT;

/**
 * FocusFinderStrategyT
 *
 * TODO: Move to sep header file?
 */
struct FocusFinderStrategyT {
  enum TypeE {
    FAST_CURVE_LOOKUP,
    _Count
  };

  static const char * asStr(const TypeE & inType) {
    switch (inType) {
    	case FAST_CURVE_LOOKUP: return "FAST_CURVE_LOOKUP";
    	default: return "<?>";
    }
  }
  MAC_AS_TYPE(Type, E, _Count);
};


class FocusFinderFactoryT {
public:
	static std::shared_ptr<FocusFinderT> getInstance(const FocusFinderStrategyT::TypeE & strategy);
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_FACTORY_H_ */
