#include "include/curve_function_gaussian.h"
#include "include/curve_parms.h"
#include "include/math_functions.h"

CurveFunctionGaussianT::CurveFunctionGaussianT(const CurveParmsT & curveParms) : CurveFunctionT(curveParms) {
  
}

std::string CurveFunctionGaussianT::getName() const {
  return "CurveFunctionGaussianT";
}

float CurveFunctionGaussianT::f(float x) const {

  float b = mCurveParms.get(IdxT::B_IDX).getValue();
  float p = mCurveParms.get(IdxT::P_IDX).getValue();
  float c = mCurveParms.get(IdxT::C_IDX).getValue();
  float w = mCurveParms.get(IdxT::W_IDX).getValue();
  
  return MathFunctionsT::gaussian(x, b, p, c, w);
}

PointFT CurveFunctionGaussianT::min() const {
  // TODO: Implement...?
  throw CurveFunctionExceptionT("CurveFunctionGaussianT::min() not implemented.");
}

PointFT CurveFunctionGaussianT::max() const {
  // TODO: Implement...?
  throw CurveFunctionExceptionT("CurveFunctionGaussianT::max() not implemented.");
}
