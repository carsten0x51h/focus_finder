#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_MEASURE_TYPE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_MEASURE_TYPE_H_

#include "enum_helper.h"

struct FocusMeasureTypeT {
  enum TypeE {
	      HFD,
	      FWHM_VERT,
	      FWHM_HORZ,
	      _Count
  };
  
		static const char * asStr(const TypeE & inType) {
			switch (inType) {
			case HFD:
			  return "HFD";
			case FWHM_VERT:
			  return "FWHM_VERT";
			case FWHM_HORZ:
			  return "FWHM_HORZ";
			default:
			  return "<?>";
		}
	}
	MAC_AS_TYPE(Type, E, _Count);
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_MEASURE_TYPE_H_ */
