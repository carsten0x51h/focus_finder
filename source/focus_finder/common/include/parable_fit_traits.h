#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_PARABLE_FIT_TRAITS_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_PARABLE_FIT_TRAITS_H_

#include <array>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "gsl_multi_fit_parms.h"
#include "curve_fit_exception.h"
#include "throw_if.h"
#include "logging.h"

/**
 * Parabel fit traits
 */
class ParabelFitTraitsT {
private:

public:
	struct CurveParamsT {
		enum TypeE {
			A_IDX = 0, B_IDX, C_IDX, _Count
		};
		struct TypeT: public std::array<float, TypeE::_Count> {
			TypeT(const gsl_vector * inVec = 0) {
				for (size_t i = 0; i < TypeE::_Count; ++i) {
					TypeE idx = static_cast<TypeE>(i);
					(*this)[i] = (inVec ? gsl_vector_get(inVec, idx) : 0);
				}
			}
		};
	};

	/* Makes a guess for a, b and c based on the supplied data */
	static void makeGuess(const GslMultiFitParmsT & inData,
			gsl_vector * guess) {
		THROW_IF(CurveFit, inData.size() < 2, "inData.size() < 2!");

		float xMin = inData.at(0).pt.x;
		float yMin = inData.at(0).pt.y;

		// Find minimum y value...
		for (size_t i = 0; i < inData.size(); ++i) {
			const DataPointT & dataPoint = inData.at(i).pt;
			if (dataPoint.y < yMin) {
				xMin = dataPoint.x;
				yMin = dataPoint.y;
			}
		}

		LOG(debug) << "xMin: " << xMin << ", yMin: " << yMin << endl;

		// Guessing of parameters a,b and c using prominent coordinates of the curve
		const float x1 = inData.at(0).pt.x; // Left value
		const float y1 = inData.at(0).pt.y;
		const float x2 = xMin;
		const float y2 = yMin;
		const float x3 = inData.back().pt.x; // Right value
		const float y3 = inData.back().pt.y;

		const float x1x2sq = x1 * x1 - x2 * x2;
		const float x1x3sq = x1 * x1 - x3 * x3;

		const float bNumerator = (y1 - y2) * x1x3sq - y1 * (x1 * x1 - x2 * x2)
				+ y3 * x1x2sq;
		const float bDenominator = (x1 - x3) * x1x2sq - (x1 - x2) * x1x3sq;

		const float bGuess = -bNumerator / bDenominator;
		const float aGuess = (y1 - y2 - bGuess * (x1 - x2)) / x1x2sq;
		const float cGuess = y2 - aGuess * x2 * x2 - bGuess * x2;

		LOG(debug) << "Guessing a=" << aGuess << ", b=" << bGuess << ", c="
				<< cGuess << endl;

		gsl_vector_set(guess, CurveParamsT::A_IDX, aGuess);
		gsl_vector_set(guess, CurveParamsT::B_IDX, bGuess);
		gsl_vector_set(guess, CurveParamsT::C_IDX, cGuess);
	}

	/* y = a * x^2 + b * x + c */
	static float fx(float x, const CurveParamsT::TypeT & inParms) {
		float a = inParms[CurveParamsT::A_IDX];
		float b = inParms[CurveParamsT::B_IDX];
		float c = inParms[CurveParamsT::C_IDX];
		float x2 = x * x;
		return (a * x2 + b * x + c);
	}

	/* Calculates f(x) = a*x^2 + b*x + c for each data point */
	static int gslFx(const gsl_vector * x, void * params, gsl_vector * f) {
		/* Store the parameter data */
		const GslMultiFitParmsT * gslParams = ((GslMultiFitParmsT*) params);

		/* Store the current coefficient values */
		CurveParamsT::TypeT resParams(x);

		/* Execute Levenberg-Marquart on f(x) */
		for (size_t i = 0; i < gslParams->size(); ++i) {
			const GslMultiFitDataT & gslData = gslParams->at(i);
			const float yi = ParabelFitTraitsT::fx((float) gslData.pt.x,
					resParams);
			gsl_vector_set(f, i, (yi - gslData.y) / gslData.sigma);
		}

		return GSL_SUCCESS;
	}

	/**
	 * Calculates the Jacobian (derivative) matrix of f(x) = a*x^2 + b*x + c for each data point.
	 * -> Fitting function f_i = [(a*x_i^2 + b*x_i + c) - y_i] / sigma_i
	 * -> J = 1/sigma * (x^2, x, 1)
	 *
	 * See: http://de.wikipedia.org/wiki/Jacobi-Matrix
	 * See: http://de.wikipedia.org/wiki/Levenberg-Marquardt-Algorithmus
	 */
	static int gslDfx(const gsl_vector * x, void * params, gsl_matrix * J) {

		/* Store parameter values */
		const GslMultiFitParmsT * gslParams = ((GslMultiFitParmsT*) params);

		/* Store current coefficients */
		float a = gsl_vector_get(x, CurveParamsT::A_IDX);
		float b = gsl_vector_get(x, CurveParamsT::B_IDX);
		float c = gsl_vector_get(x, CurveParamsT::C_IDX);

		for (size_t i = 0; i < gslParams->size(); ++i) {
			const GslMultiFitDataT & gslData = gslParams->at(i);
			const float oneBySigma = 1.0f / gslData.sigma;
			const float x = gslData.pt.x;

			gsl_matrix_set(J, i, CurveParamsT::A_IDX, oneBySigma * x * x);
			gsl_matrix_set(J, i, CurveParamsT::B_IDX, oneBySigma * x);
			gsl_matrix_set(J, i, CurveParamsT::C_IDX, oneBySigma);
		}

		return GSL_SUCCESS;
	}

	/* Invokes f(x) and f'(x) */
	static int gslFdfx(const gsl_vector * x, void * params, gsl_vector * f,
			gsl_matrix * J) {
		// TODO: Just for optmization we may directly compute f -> df... for now we just call the respective functions...
		ParabelFitTraitsT::gslFx(x, params, f);
		ParabelFitTraitsT::gslDfx(x, params, J);

		return GSL_SUCCESS;
	}
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_PARABLE_FIT_TRAITS_H_*/
