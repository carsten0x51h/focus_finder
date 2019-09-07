#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_FITTING_CURVE_FACTORY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_FITTING_CURVE_FACTORY_H_

#include <memory>

#include "fitting_curve_type.h"
#include "lm_fitting_curve.h"


class LmFittingCurveFactoryT {
public:
	static std::shared_ptr<LmFittingCurveT> getInstance(const FittingCurveTypeT::TypeE & type);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FITTING_CURVE_FACTORY_H_*/
