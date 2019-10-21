#include "include/curve_function_hyperbolic.h"
#include "include/curve_parms.h"
#include "include/math_functions.h"

CurveFunctionHyperbolicT::CurveFunctionHyperbolicT(const CurveParmsT & curveParms) : CurveFunctionT(curveParms) {
  
}

std::string CurveFunctionHyperbolicT::getName() const {
  return "CurveFunctionHyperbolicT";
}

float CurveFunctionHyperbolicT::f(float x) const {

  float a = mCurveParms.get(IdxT::A_IDX).getValue();
  float b = mCurveParms.get(IdxT::B_IDX).getValue();
  float c = mCurveParms.get(IdxT::C_IDX).getValue();
  float d = mCurveParms.get(IdxT::D_IDX).getValue();;
  
  return MathFunctionsT::hyperbolic(x, a, b, c, d);
}
