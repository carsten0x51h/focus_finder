#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_GAUSSIAN_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_GAUSSIAN_H_

#include "curve_function.h"
#include "curve_parms.h"
#include "enum_helper.h"

class CurveFunctionGaussianT : public CurveFunctionT {

 public:
  struct IdxT {
    enum TypeE {
		B_IDX = 0, P_IDX, C_IDX, W_IDX, _Count
    };

    static const char * asStr(const TypeE & inType) {
      switch (inType) {
      case B_IDX:
	return "B_IDX";
      case P_IDX:
	return "P_IDX";
      case C_IDX:
	return "C_IDX";
      case W_IDX:
	return "W_IDX";
      default:
	return "<?>";
      }
    }
    MAC_AS_TYPE(Type, E, _Count);
  };

  CurveFunctionGaussianT(const CurveParmsT & curveParms);
  std::string getName() const;
  float f(float x) const;
  PointFT min() const;
  PointFT max() const;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_GAUSSIAN_H_*/
