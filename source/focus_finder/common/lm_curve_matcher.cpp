#include <memory>
#include <string>

#include <gsl/gsl_vector.h>

#include "include/lm_curve_matcher.h"
#include "include/lm_fitting_curve.h"

LmCurveMatcherT::LmCurveMatcherT(std::shared_ptr<LmFittingCurveT> lmFittingCurve) :
		mLmFittingCurve(lmFittingCurve) {

}

CurveParmsT LmCurveMatcherT::fillCurveParms(
		const gsl_vector * resultVec) const {
	CurveParmsT curveParms(resultVec->size);
	auto parmNames = mLmFittingCurve->getParmNames();

	for (size_t idx = 0; idx < resultVec->size; ++idx) {
		std::string parmName = parmNames.at(idx);
		float value = gsl_vector_get(resultVec, idx);

		curveParms[idx] = CurveParmT(parmName, value);
	}

	return curveParms;
}
