#include <memory>

#include "include/lm_fitting_curve.h"
#include "include/lm_fitting_curve_factory.h"
#include "include/lm_fitting_curve_hyperbolic.h"
#include "include/lm_fitting_curve_parabolic.h"
#include "include/lm_fitting_curve_gaussian.h"
#include "include/lm_fitting_curve_hyperbolic_pos_only.h"

// NOTE / TODO: In case the "LmFittingCurveHyperbolicPosOnlyT" is really required AND it should be constructed by this factory, we could supply parameters "a" and "b" using variadic templates. See the following link for an example:
// https://stackoverflow.com/questions/28256550/factory-pattern-using-variadic-template
std::shared_ptr<LmFittingCurveT> LmFittingCurveFactoryT::getInstance(const FittingCurveTypeT::TypeE & fittingCurveType) {

	switch(fittingCurveType) {
	case FittingCurveTypeT::HYPERBOLIC:
		return std::make_shared<LmFittingCurveHyperbolicT>();

	case FittingCurveTypeT::LINEAR_SLOPE:
		return nullptr; // TODO

	case FittingCurveTypeT::PARABOLIC:
		return std::make_shared<LmFittingCurveParabolicT>();

	case FittingCurveTypeT::GAUSSIAN:
		return std::make_shared<LmFittingCurveGaussianT>();
		
	case FittingCurveTypeT::HYPERBOLIC_POS_ONLY:
		return std::make_shared<LmFittingCurveHyperbolicPosOnlyT>();

	default:
		return nullptr;
	}
}
