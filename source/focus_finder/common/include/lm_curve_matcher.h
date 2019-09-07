#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_CURVE_MATCHER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_CURVE_MATCHER_H_

#include <memory>
#include <vector>
#include <string>
#include <functional>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multifit_nlin.h>

#include "logging.h"
#include "exception.h"
#include "point.h"

#include "lm_curve_matcher_summary.h"
#include "lm_curve_matcher_exception.h"
#include "lm_fitting_curve.h"
#include "lm_fitting_parms_container.h"
#include "gsl_multi_fit_parms.h"

#include "curve_fit_parms.h"
#include "curve_fit_summary.h"
#include "curve_parms.h"


class LmCurveMatcherT {
public:
	LmCurveMatcherT(std::shared_ptr<LmFittingCurveT> lmFittingCurve);

  // TODO: Replace CurveFitParmsT & by LmCurveMatcherParmsT... (new class which does not hold outlier stuff... -> also reduced dependencies...)
	template<typename RngT>
	CurveParmsT fit(const RngT & rng, const CurveFitParmsT & curveFitParms,
			LmCurveMatcherSummaryT * outLmCurveMatcherSummary = nullptr) {

		CurveParmsT curveParms;
		LmCurveMatcherSummaryT lmCurveMatcherSummary;
		size_t numPoints = std::distance(rng.begin(), rng.end());
		size_t numCurveParams = mLmFittingCurve->getParmNames().size();

		LOG(debug) << "LmCurveMatcher::fit() - numPoints: " << numPoints << std::endl;
		
		GslMultiFitParmsT gslMultiFitParms;//(numPoints);
		LmFittingParmsContainerT parmsContainer(& gslMultiFitParms, mLmFittingCurve.get());

		//TODO: No longer required? gslMultiFitParms.reserve(numPoints);

		// Fill in the parameters
		for (auto it = rng.begin(); it != rng.end(); ++it) {
		  
		  // WAS:
		  // gslMultiFitParms[idx].y = dataPoint.y();
		  // gslMultiFitParms[idx].sigma = 0.1f;
		  // gslMultiFitParms[idx].pt = dataPoint;
		  gslMultiFitParms.push_back(GslMultiFitDataT(it->y() /*y*/, 0.1 /*sigma*/, *it /*data point*/));
		}

		// Fill in function info
		// See http://www.csse.uwa.edu.au/programming/gsl-1.0/gsl-ref_35.html
		gsl_multifit_function_fdf f;
		f.f = & LmFittingCurveT::gslFx;
		f.df = & LmFittingCurveT::gslDfx;
		f.fdf = & LmFittingCurveT::gslFdfx;
		f.n = numPoints;
		f.p = numCurveParams;
		f.params = & parmsContainer;

		gsl_vector * guess = gsl_vector_alloc(numCurveParams); // Allocate the guess vector

		mLmFittingCurve->makeGuess(gslMultiFitParms, guess); // Make initial guesses

		// Create a Levenberg-Marquardt solver with n data points and m parameters
		gsl_multifit_fdfsolver * solver = gsl_multifit_fdfsolver_alloc(
				gsl_multifit_fdfsolver_lmsder, numPoints, numCurveParams);

		gsl_multifit_fdfsolver_set(solver, &f, guess); // Initialize the solver

		int status;
		size_t iter = 0;
		
		// Iterate to to find a result
		// Status will be 0 in case of success, otherwise value != 0 (GSL error code).
		do {
			iter++;
			status = gsl_multifit_fdfsolver_iterate(solver); // returns 0 in case of success

			if (status) {
				break;
			}

			// See http://www.csse.uwa.edu.au/programming/gsl-1.0/gsl-ref_35.html
			status = gsl_multifit_test_delta(solver->dx, solver->x, curveFitParms.epsabs,
					curveFitParms.epsrel);

		} while (GSL_CONTINUE == status && iter < curveFitParms.numMaxIterations);

		// Store the result into the summary
		lmCurveMatcherSummary.numIterationsRequired = iter;
		lmCurveMatcherSummary.statusCode = status;
		lmCurveMatcherSummary.statusMsg = gsl_strerror(status);

		// In case of success, fill the resulting curve parameters into the generic CurveParmsT structure
		if (GSL_SUCCESS == status) {
			curveParms = fillCurveParms(solver->x);
		}

		// In any case cleanup - Free GSL memory
		gsl_multifit_fdfsolver_free(solver);
		gsl_vector_free(guess);

		if (GSL_SUCCESS != status) {
			std::stringstream ss;
			ss << "GSL fit failed. Reason: " << gsl_strerror(status)
					<< " (status code: " << status << ")." << std::endl;

			throw LmCurveMatcherExceptionT(ss.str(), lmCurveMatcherSummary);
		}

		// If user requested, store the matching summary
		if (outLmCurveMatcherSummary != nullptr) {
			*outLmCurveMatcherSummary = lmCurveMatcherSummary;
		}

		return curveParms;
	}

private:
	CurveParmsT fillCurveParms(const gsl_vector * resultVec) const;

	std::shared_ptr<LmFittingCurveT> mLmFittingCurve;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_CURVE_MATCHER_H_*/

//class LevenbergMarquartCurveFitAlgorithmT {
//private:
//
//	class OutlierDetectorStandardDeviationT {
//	private:
//		mResidualFunction;
//		float mOutlierBoundary;
//
//
//		void updateOutlierBoundary(const RngT & dataPointsToFit) {
//
//			// Calculate outlier boundary - just the first time since otherwise q3 may get smaller and smaller
//			// with each iteration and produce new outliers again.
//			// TODO: This is most probably not the correct calculation which is suggested by "OutlierDetectorStandardDeviationT"...
//			mOutlierBoundary = mBoundaryFac * quantile<float>(dataPointsToFit | transformed(residualFunction), 0.75) /*q3*/;
//		}
//
//	public:
//		OutlierDetectorStandardDeviationT(float inBoundaryFac) : mBoundaryFac(inBoundaryFac) {
//		}
//
//		updateOutlierBoundary(const RngT & dataPointsToFit, );
//		void setResidualFunction();
//
//
//		// TODO: throw if number of required data points is no longer given
//		template<typename Rng>
//		std::vector< std::pair<typename Rng::value_type, float> > detect(const RngT & dataPointsToFit) {
//
//			auto mapPointToPointResidualPairFunction = [&](const auto & p) { return std::make_pair(p, residualFunction(p)); };
//			auto greaterThanBoundaryFunction = [&] (const auto & pair) { return pair.second > mOutlierBoundary; };
//
//			auto outliers = dataPointsToFit
//							| transformed(mapPointToPointResidualPairFunction)
//							| filtered(greaterThanBoundaryFunction);
//
//			return outliers;
//
//
//			// Sort criteria: Smallest residual first, if residual equal, greatest y first (because data points further
//			// outside V-Curve are potentially more important). -> TODO: Is that true?
////			auto less =
////					[&](const PointFT & p1, const PointFT & p2) -> bool {
////						float residual1 = residualFunction(p1);
////						float residual2 = residualFunction(p2);
////						return ((fabs(residual1 - residual2) > std::numeric_limits<float>::epsilon()) ? residual1 < residual2 : p1.y() > p2.y());
////					};
//
////			boost::sort(dataPointsToFit, less);
//
//			// itUp points to first element in dataPointsToFit > outlierBoundary
////			auto itUp = boost::range::upper_bound(dataPointsToFit,
////					outlierBoundary, [&](float rhs, const PointFT & p) -> bool {
////						return residualFunction(p) > rhs;
////					});
//
////			numValidDataPoints = std::distance(dataPointsToFit->begin(), itUp);
////			numCurrOutliers = dataPointsToFit->size() - numValidDataPoints;
////			insufficientDataPoints = (numValidDataPoints
////					< numMinRequiredDataPoints);
//
////			size_t limitIdx = (
////					insufficientDataPoints ?
////							numMinRequiredDataPoints : numValidDataPoints);
//
//			// Add outliers to outlier stats
////			if (outliers != nullptr) {
////				// TODO
////				//			std::transform(itUp, dataPointsToFit->end(),
////				//					std::back_inserter(outlierStats),
////				//					[&](const PointFT & p) {return std::make_pair(p, residualFunction(p));});
////			}
//
//			// "Remove" elements behind limitIdx by simply resizing the vector
//			// https://stackoverflow.com/questions/21493414/keeping-the-first-n-elements-of-a-stdvector-and-removing-the-rest
//			//dataPointsToFit->resize(limitIdx);
//		}
//	}
//
//
//
//	/**
//	 * TODO: See https://docs.oracle.com/cd/E57185_01/CBREG/ch06s02s10s01.html for 3 outlier detection methods
//	 * TODO: Improve init of dataPointsToFit by reserving space?!
//	 * TODO: We may pass boundary detection rule as lambda instead of factor 1.5 to fitVCurve()...
//	 * TODO: Evaluate return value (status) of fitGslLevenbergMarquart() call -> gsl_strerror(status)?
//	 * TODO: What to pass as epsAbs and epsRel?
//	 */
//	template<typename RngT>
//	CurveParamsT fitCurveWithOutlierDetection(const RngT & rng,
//			const FittingCurveT & fittingCurve,
//			std::shared_ptr<CurveFitDetailsT> curveFitDetails,
//			std::shared_ptr<OutlierDetectorT> outlierDetector = nullptr) {
//
//		using namespace boost::adaptors;
//
//		std::vector<OutlierT> outliers;
//
//		size_t numIterations = 0;
//		size_t numCurrOutliersRemoved = 0, numValidDataPoints;
//		size_t numInitialDataPoints = std::distance(rng.begin(), rng.end());
//
//		size_t numMinRequiredDataPoints = numInitialDataPoints
//				* (1.0f - mMaxAcceptedOutliersPerc / 100.0f);
//
//		bool insufficientDataPoints;
//
//
//		// Take an initial copy of the incoming data points.
//		RngT dataPointsToFit;
//		boost::range::copy(rng, std::back_inserter(dataPointsToFit));
//
//
//
//		// Initialize hyperbolCurveParms - unfortunately required to calc q3 outside the loop.
//
//		// TODO: Error handling... -> throw? exc. should contain all the details (curveFitDetails)!
//		CurveParamsT curveParams = fitCurve(dataPointsToFit, fittingCurve, curveFitDetails);
//
//
//		// Lambda-function to calculate residual
//		// TODO: Could be passed from outside... in that case Point, FitingCurve and CurveParamsT would be passed in.
//		auto residualFunction =
//				[&](const PointFT & p) { return std::fabs(p.y() - fittingCurve->fx(p.x(), curveParams)); };
//
//
//		// IDEA: Use C++ variadic tmpl. to pass ctor parameters into getInstance() for each possuble object type.
//		//       -> see https://www.bfilipek.com/2016/03/nice-c-factory-implementation-2.html
//		outlierDetector->setResidualFunction(residualFunction);
//
//		// OR: outlierDetector->updateResidualBoundary(dataPointsToFit, curveParams);
//		//, const FittingCurveT & fittingCurve, const CurveParamsT & curveParams
//
//				if (updateOutlierBoundary) {
//
//		}
//
//
//
//		do {
//			++numIterations;
//
//			// TODO: Error handling?!
//			curveParams = fitCurve(dataPointsToFit, fittingCurve, curveFitDetails);
//
//			auto outliers = outlierDetector->detect(dataPointsToFit);
//
//
//		} while (numCurrOutliersRemoved > 0 && ! insufficientDataPoints);
//
//		RESULT
//		//std::shared_ptr<OutlierDetectorSummaryT> outlierDetectorSummary;	// < In case outlier detection is enabled, contains additoinal details about outlier detection result
//
//		// Lambda to build fitting stats
//		// TODO: building API?!
//		auto buildFittingStats = [&]() -> FittingStatsT {
//			FittingStatsT fittingStats;
//			fittingStats.setOutlierStats(outlierStats);
//			fittingStats.setMatchedDataPoints(dataPointsToFit);
//			fittingStats.setCurveParms(curveParms);
//			fittingStats.setOutlierBoundary(outlierBoundary);
//			fittingStats.setNumIterations(numIterations);
//			fittingStats.setNumInitialDataPoints(numInitialDataPoints);
//			fittingStats.setNumMinRequiredDataPoints(numMinRequiredDataPoints);
//			fittingStats.setNumValidDataPoints(numValidDataPoints);
//			fittingStats.setInsufficientDataPoints(insufficientDataPoints);
//			return fittingStats;
//		};
//
//		// Check if fitting was finally successful
//		if (insufficientDataPoints) {
//			std::stringstream ss;
//			ss
//					<< "Not enough data points with sufficient model accuracy. Require at least "
//					<< numMinRequiredDataPoints << " datapoints but only have "
//					<< dataPointsToFit.size() << " / " << numInitialDataPoints
//					<< "." << std::endl;
//
//			throw CurveFitExceptionT(ss.str().c_str());
//		}
//
//		if (outFittingStats != nullptr) {
//			*outFittingStats = buildFittingStats();
//		}
//
//		return curveParms;
//	}
//
//public:
//	LevenbergMarquartCurveFitAlgorithmT(float inEpsAbs, float inEpsRel,
//			size_t inNumMaxIter);
//
//	/**
//	 * See http://en.wikipedia.org/wiki/Approximation_error for expl. of rel and abs errors.
//	 */
//	template<typename RngT>
//	std::shared_ptr<CurveFitResultT> fit(const RngT & rng,
//			const FittingCurveT & fittingCurve,
//			OutlierDetectorTypeT::TypeE outlierDetectorType) const {
//
//		auto curveFitResult = std::make_shared<CurveFitResultT>();
//
//		if (outlierDetectorType == OutlierDetectorTypeT::NONE) {
//			fitCurve(rng, fittingCurve, curveFitResult);
//		} else {
//			fitCurveWithOutlierDetection(rng, fittingCurve, outlierDetectorType,
//					curveFitResult);
//		}
//
//		return curveFitResult;
//	}
//
////	typedef std::pair<PointFT, float> OutlierStatsEntryT;
////	typedef std::vector<OutlierStatsEntryT> OutlierStatsT;
////	std::shared_ptr<CurveFitResultT> fit(const RngT & rng,
////			const CurveFittingCfgT & curveFittingCfg) const
//
//};

//#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LEVENBERG_MARQUART_CURVE_FIT_ALGORITHM_H_*/
