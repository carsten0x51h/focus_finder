#include <memory>

#include "include/lm_fitting_curve.h"
#include "include/lm_fitting_curve_factory.h"
#include "include/lm_fitting_curve_hyperbolic.h"
#include "include/lm_fitting_curve_parabolic.h"
#include "include/lm_fitting_curve_gaussian.h"

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

	default:
		return nullptr;
	}
}
