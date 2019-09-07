#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FITTING_CURVE_TYPE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FITTING_CURVE_TYPE_H_


#include "enum_helper.h"

struct FittingCurveTypeT {
  enum TypeE {
    HYPERBOLIC,
    PARABOLIC,
	GAUSSIAN,
    LINEAR_SLOPE,
    _Count
  };

  static const char * asStr(const TypeE & inType) {
    switch (inType) {
    	case HYPERBOLIC: return "HYPERBOLIC";
    	case PARABOLIC: return "PARABOLIC";
    	case GAUSSIAN: return "GAUSSIAN";
    	case LINEAR_SLOPE: return "LINEAR_SLOPE";
    	default: return "<?>";
    }
  }
  MAC_AS_TYPE(Type, E, _Count);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FITTING_CURVE_TYPE_H_*/
