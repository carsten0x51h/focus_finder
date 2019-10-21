#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_PARABOLIC_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_PARABOLIC_H_

#include "curve_function.h"
#include "curve_parms.h"
#include "enum_helper.h"

class CurveFunctionParabolicT : public CurveFunctionT {

public:
  struct IdxT {
    enum TypeE {
		A_IDX = 0, B_IDX, C_IDX, _Count
    };

    static const char * asStr(const TypeE & inType) {
      switch (inType) {
      case A_IDX:
	return "A_IDX";
      case B_IDX:
	return "B_IDX";
      case C_IDX:
	return "C_IDX";
      default:
	return "<?>";
      }
    }
    MAC_AS_TYPE(Type, E, _Count);
  };

  CurveFunctionParabolicT(const CurveParmsT & curveParms);
  std::string getName() const;
  float f(float x) const;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_PARABOLIC_H_*/
