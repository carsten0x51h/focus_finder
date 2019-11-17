#include "include/curve_function_hyperbolic.h"
#include "include/curve_parms.h"
#include "include/math_functions.h"
#include "include/point.h"

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


float CurveFunctionHyperbolicT::f_inv(float x) const {
  float a = mCurveParms.get(IdxT::A_IDX).getValue();
  float b = mCurveParms.get(IdxT::B_IDX).getValue();
  float c = mCurveParms.get(IdxT::C_IDX).getValue();
  float d = mCurveParms.get(IdxT::D_IDX).getValue();;
  
  return MathFunctionsT::hyperbolic_inv(x, a, b, c, d);
}


PointFT CurveFunctionHyperbolicT::min() const {
  // For the hyperbolic curve, "c" is the minimum x value.
  // TODO: Is this always true? Can't it be the max. as well?
  float x = mCurveParms.get(IdxT::C_IDX).getValue();
  return PointFT(x, f(x));
}

PointFT CurveFunctionHyperbolicT::max() const {
  // TODO: Implement...?
  throw CurveFunctionExceptionT("CurveFunctionHyperbolicT::max() not implemented.");
}

