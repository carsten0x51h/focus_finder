#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_BOUNDARY_LOCATION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_BOUNDARY_LOCATION_H_

#include "enum_helper.h"

struct BoundaryLocationT {
  enum TypeE {
    INSIDE_BOUNDARY,
    OUTSIDE_BOUNDARY,
    WITHIN_BOUNDARY_RANGE,
    _Count
  };

  static const char * asStr(const TypeE & inType) {
    switch (inType) {
    	case INSIDE_BOUNDARY: return "INSIDE_BOUNDARY";
    	case OUTSIDE_BOUNDARY: return "OUTSIDE_BOUNDARY";
    	case WITHIN_BOUNDARY_RANGE: return "WITHIN_BOUNDARY_RANGE";
    	default: return "<?>";
    }
  }
  MAC_AS_TYPE(Type, E, _Count);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_BOUNDARY_LOCATION_H_*/
