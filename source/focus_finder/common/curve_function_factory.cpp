#include <memory>

#include "include/curve_function_factory.h"
#include "include/fitting_curve_type.h"
#include "include/curve_parms.h"
#include "include/curve_function.h"
#include "include/curve_function_hyperbolic.h"
#include "include/curve_function_parabolic.h"
#include "include/curve_function_gaussian.h"

std::shared_ptr<CurveFunctionT> CurveFunctionFactoryT::getInstance(FittingCurveTypeT::TypeE curveType, const CurveParmsT & curveParms) {
  switch(curveType) {
  case FittingCurveTypeT::HYPERBOLIC:
    return std::make_shared<CurveFunctionHyperbolicT>(curveParms);
  case FittingCurveTypeT::PARABOLIC:
    return std::make_shared<CurveFunctionParabolicT>(curveParms);
  case FittingCurveTypeT::GAUSSIAN:
    return std::make_shared<CurveFunctionGaussianT>(curveParms);
  default:
    return nullptr;
  }
}
