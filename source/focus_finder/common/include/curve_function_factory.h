#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_FACTORY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_FACTORY_H_

#include <memory>

#include "fitting_curve_type.h"
#include "curve_parms.h"
#include "curve_function.h"

class CurveFunctionFactoryT {
 public:
  static std::shared_ptr<CurveFunctionT> getInstance(FittingCurveTypeT::TypeE fittingCurveType, const CurveParmsT & curveParms);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_FACTORY_H_*/
