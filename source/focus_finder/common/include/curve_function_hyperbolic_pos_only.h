#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_HYPERBOLIC_POS_ONLY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_HYPERBOLIC_POS_ONLY_H_

#include "curve_function.h"
#include "curve_parms.h"
#include "enum_helper.h"
#include "point.h"

class CurveFunctionHyperbolicPosOnlyT : public CurveFunctionT {
 public:
  struct IdxT {
    enum TypeE {
		C_IDX = 0, D_IDX, _Count
    };

    static const char * asStr(const TypeE & inType) {
      switch (inType) {
      case C_IDX:
	return "C_IDX";
      case D_IDX:
	return "D_IDX";
      default:
	return "<?>";
      }
    }
    MAC_AS_TYPE(Type, E, _Count);
  };

  CurveFunctionHyperbolicPosOnlyT(const CurveParmsT & curveParms);
  std::string getName() const;
  float f(float x) const;
  float f_inv(float x) const;
  PointFT min() const;
  PointFT max() const;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_HYPERBOLIC_POS_ONLY_H_*/
