#include "include/curve_function_hyperbolic_pos_only.h"
#include "include/curve_parms.h"
#include "include/math_functions.h"
#include "include/point.h"

CurveFunctionHyperbolicPosOnlyT::CurveFunctionHyperbolicPosOnlyT(const CurveParmsT & curveParms) : CurveFunctionT(curveParms) {
  
}

std::string CurveFunctionHyperbolicPosOnlyT::getName() const {
  return "CurveFunctionHyperbolicPosOnlyT";
}

float CurveFunctionHyperbolicPosOnlyT::f(float x) const {

  float a = 15268.2; // HACK! mCurveParms.get(IdxT::A_IDX).getValue();
  float b = 11.074; // HACK! mCurveParms.get(IdxT::B_IDX).getValue();
  float c = mCurveParms.get(IdxT::C_IDX).getValue();
  float d = mCurveParms.get(IdxT::D_IDX).getValue();;
  
  return MathFunctionsT::hyperbolic(x, a, b, c, d);
}


float CurveFunctionHyperbolicPosOnlyT::f_inv(float x) const {
  float a = 15268.2; // HACK! mCurveParms.get(IdxT::A_IDX).getValue();
  float b = 11.074; // HACK! mCurveParms.get(IdxT::B_IDX).getValue();
  float c = mCurveParms.get(IdxT::C_IDX).getValue();
  float d = mCurveParms.get(IdxT::D_IDX).getValue();;
  
  return MathFunctionsT::hyperbolic_inv(x, a, b, c, d);
}


PointFT CurveFunctionHyperbolicPosOnlyT::min() const {
  // For the hyperbolic curve, "c" is the minimum x value.
  // TODO: Is this always true? Can't it be the max. as well?
  float x = mCurveParms.get(IdxT::C_IDX).getValue();
  return PointFT(x, f(x));
}

PointFT CurveFunctionHyperbolicPosOnlyT::max() const {
  // TODO: Implement...?
  throw CurveFunctionExceptionT("CurveFunctionHyperbolicPosOnlyT::max() not implemented.");
}
