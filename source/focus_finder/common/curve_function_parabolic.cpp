#include "include/curve_function_parabolic.h"
#include "include/curve_parms.h"
#include "include/math_functions.h"
#include "include/point.h"

CurveFunctionParabolicT::CurveFunctionParabolicT(const CurveParmsT & curveParms) : CurveFunctionT(curveParms) {
  
}

std::string CurveFunctionParabolicT::getName() const {
  return "CurveFunctionParabolicT";
}

float CurveFunctionParabolicT::f(float x) const {

  float a = mCurveParms.get(IdxT::A_IDX).getValue();
  float b = mCurveParms.get(IdxT::B_IDX).getValue();
  float c = mCurveParms.get(IdxT::C_IDX).getValue();
  
  return MathFunctionsT::parabolic(x, a, b, c);
}

PointFT CurveFunctionParabolicT::min() const {
  // TODO: Implement...
  
  // NOTE: Parabel has 3 cases:
  //   - a > 0: min
  //   - a < 0: max
  //   - a = 0: none, no real parabel
  //
  // -> Function needs to be converted to "Scheitelpunktsform"! -> then d and e directly give the (x,y) pos of the extremum.
  throw CurveFunctionExceptionT("CurveFunctionParabolicT::min() not implemented.");
}

PointFT CurveFunctionParabolicT::max() const {
  // TODO: Implement...
  throw CurveFunctionExceptionT("CurveFunctionParabolicT::max() not implemented.");
}
