#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_THRESHOLDING_ALGORITHM_TYPE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_THRESHOLDING_ALGORITHM_TYPE_H_

#include "enum_helper.h"

struct ThresholdingAlgorithmTypeT {
  enum TypeE {
	      OTSU,
	      MAX_ENTROPY,
	      _Count
  };
  
		static const char * asStr(const TypeE & inType) {
			switch (inType) {
			case OTSU:
			  return "OTSU";
			case MAX_ENTROPY:
			  return "MAX_ENTROPY";
			default:
			  return "<?>";
		}
	}
	MAC_AS_TYPE(Type, E, _Count);
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_THRESHOLDING_ALGORITHM_TYPE_H_ */
