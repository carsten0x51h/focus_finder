#include "include/curve_function_parabolic.h"
#include "include/curve_parms.h"
#include "include/math_functions.h"

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
