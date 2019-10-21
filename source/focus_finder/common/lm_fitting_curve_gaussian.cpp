#include <vector>
#include <math.h>

#include "include/math_functions.h"
#include "include/throw_if.h"
#include "include/logging.h"
#include "include/lm_fitting_curve_gaussian.h"
#include "include/lm_curve_matcher_exception.h"

std::string LmFittingCurveGaussianT::getName() const {
	return "LmFittingCurveGaussianT";
}

std::vector<std::string>
LmFittingCurveGaussianT::getParmNames() const { return LmFittingCurveT::getParmNames<IdxT>(); }

/**
 * In this case we just initialize all parameters with 1.
 */
void LmFittingCurveGaussianT::makeGuess(const GslMultiFitParmsT & inData,
		gsl_vector * guess) {
	THROW_IF(LmCurveMatcher, inData.size() < 2, "inData.size() < 2!");

	size_t numDataPoints = inData.size();
	float y_mean = 0;
	float y_max = inData.at(0).pt.y();
	float c = inData.at(0).pt.x();

	for (size_t i = 0; i < numDataPoints; ++i) {
		const PointFT & dataPoint = inData.at(i).pt;

		y_mean += dataPoint.y();

		if (y_max < dataPoint.y()) {
			y_max = dataPoint.y();
			c = dataPoint.x();
		}
	}

	y_mean /= (float) numDataPoints;
	float w = (inData.at(numDataPoints - 1).pt.x() - inData.at(0).pt.x())
			/ 10.0;

	gsl_vector_set(guess, IdxT::B_IDX, y_mean);
	gsl_vector_set(guess, IdxT::P_IDX, y_max);
	gsl_vector_set(guess, IdxT::C_IDX, c);
	gsl_vector_set(guess, IdxT::W_IDX, w);
}

/* y = b + p * exp(-0.5f * ((t - c) / w) * ((t - c) / w)) */
float LmFittingCurveGaussianT::fx(float x, const gsl_vector * curveParms) const {
	float b = gsl_vector_get(curveParms, IdxT::B_IDX);
	float p = gsl_vector_get(curveParms, IdxT::P_IDX);
	float c = gsl_vector_get(curveParms, IdxT::C_IDX);
	float w = gsl_vector_get(curveParms, IdxT::W_IDX);

	return MathFunctionsT::gaussian(x, b, p, c, w);
}


/* Calculates fitting funtion for H(x) for each data point. */
int LmFittingCurveGaussianT::gslFx(const gsl_vector * curveParms, const GslMultiFitParmsT * gslParms,
		gsl_vector * outResultVec) {

  LOG(debug) << "LmFittingCurveGaussianT::gslFx() - gslParms->size(): " << gslParms->size() << ", outResultVec->size: " << outResultVec->size << std::endl;

	//Execute Levenberg-Marquart on f(x)
	for (size_t i = 0; i < gslParms->size(); ++i) {
		const GslMultiFitDataT & gslData = gslParms->at(i);
		float yi = fx(gslData.pt.x(), curveParms);

		LOG(trace)
		<< "Gaussian - gslFx - i: " << i << ", gslData.pt.x: "
				<< gslData.pt.x() << ", yi: " << yi << ", gslData.y: "
				<< gslData.pt.y() << ", gslData.sigma: " << gslData.sigma
				<< std::endl;

		gsl_vector_set(outResultVec, i, (yi - gslData.y) / gslData.sigma);
	}
	return GSL_SUCCESS;
}

/* Calculates the Jacobian (derivative) matrix  */
int LmFittingCurveGaussianT::gslDfx(const gsl_vector * x, const GslMultiFitParmsT * gslParms, gsl_matrix * J) {

	// Store current coefficients
	float p = gsl_vector_get(x, IdxT::P_IDX);
	float c = gsl_vector_get(x, IdxT::C_IDX);
	float w = gsl_vector_get(x, IdxT::W_IDX);

	// Store non-changing calculations
	float w2 = w * w;
	float w3 = w2 * w;

	for (size_t i = 0; i < gslParms->size(); ++i) {
		const GslMultiFitDataT & gslData = gslParms->at(i);
		float x_minus_c = (gslData.pt.x() - c);
		float e = exp(-0.5F * (x_minus_c / w) * (x_minus_c / w));

		gsl_matrix_set(J, i, IdxT::B_IDX, 1 / gslData.sigma);
		gsl_matrix_set(J, i, IdxT::P_IDX, e / gslData.sigma);
		gsl_matrix_set(J, i, IdxT::C_IDX,
				(p * e * x_minus_c) / (gslData.sigma * w2));
		gsl_matrix_set(J, i, IdxT::W_IDX,
				(p * e * x_minus_c * x_minus_c) / (gslData.sigma * w3));
	}

	return GSL_SUCCESS;
}

/* Invokes f(x) and f'(x) */
int LmFittingCurveGaussianT::gslFdfx(const gsl_vector * x, const GslMultiFitParmsT * gslParms, gsl_vector * f,
		gsl_matrix * J) {
	gslFx(x, gslParms, f);
	gslDfx(x, gslParms, J);

	return GSL_SUCCESS;
}
