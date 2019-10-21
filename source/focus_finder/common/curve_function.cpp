#include "include/curve_function.h"
#include "include/curve_parms.h"

CurveFunctionT::CurveFunctionT(const CurveParmsT & curveParms) : mCurveParms(curveParms) {
  
}

const CurveParmsT & CurveFunctionT::getCurveParms() const {
  return mCurveParms;
}
