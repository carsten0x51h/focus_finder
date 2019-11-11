#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_HALF_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_HALF_H_

#include "enum_helper.h"

struct CurveHalfT {
  enum TypeE {
	      LEFT_HALF, RIGHT_HALF, _Count
  };
  
  static const char * asStr(const TypeE & inType) {
    switch (inType) {
    case LEFT_HALF:
      return "LEFT_HALF";
    case RIGHT_HALF:
      return "RIGHT_HALF";
    default:
      return "<?>";
    }
  }
  MAC_AS_TYPE(Type, E, _Count);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_HALF_H_*/
