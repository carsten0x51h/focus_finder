#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_STRATEGY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_STRATEGY_H_

#include "enum_helper.h"

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


#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_STRATEGY_H_*/
