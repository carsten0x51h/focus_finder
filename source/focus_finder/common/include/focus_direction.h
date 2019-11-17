#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_DIRECTION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_DIRECTION_H_

#include "enum_helper.h"

struct FocusDirectionT {
  enum TypeE {
    INWARD,
    OUTWARD,
    _Count
  };

  static const char * asStr(const TypeE & inType) {
    switch (inType) {
    	case INWARD: return "INWARD";
    	case OUTWARD: return "OUTWARD";
    	default: return "<?>";
    }
  }

  static TypeE invert(const TypeE & inType) {
    return (inType == FocusDirectionT::INWARD ? FocusDirectionT::OUTWARD : FocusDirectionT::INWARD);
  }

  MAC_AS_TYPE(Type, E, _Count);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_DIRECTION_H_*/
