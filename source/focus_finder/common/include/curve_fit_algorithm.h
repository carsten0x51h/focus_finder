#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FIT_ALGORITHM_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FIT_ALGORITHM_H_

#include <vector>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>

#include "throw_if.h"
#include "quantile.h"
#include "point_with_residual.h"

#include "fitting_curve_type.h"
#include "curve_fit_parms.h"
#include "curve_fit_summary.h"
#include "curve_fit_exception.h"
#include "curve_parms.h"
#include "outlier_filter.h"

#include "lm_fitting_curve_factory.h"
#include "lm_fitting_curve.h"
#include "lm_curve_matcher.h"
#include "lm_curve_matcher_summary.h"

// TODO: Maybe add a constructor which takes FittingCurveTypeT::TypeE, then create the LmFittingCurveT just once...
//        -> also the CurveMatcherT.
class CurveFitAlgorithmT {
public:
	template<typename Rng>
	static CurveParmsT fitCurve(FittingCurveTypeT::TypeE ct, Rng dps,
			const CurveFitParmsT & curveFitParms,
			CurveFitSummaryT * outSummary = nullptr) {
	  std::shared_ptr<LmFittingCurveT> fc = LmFittingCurveFactoryT::getInstance(ct);
		LmCurveMatcherT cm(fc);
		OutlierFilterT outlierFilter;
		CurveParmsT cp;
		CurveFitSummaryT summary;

		size_t numInitialDataPoints = std::distance(dps.begin(), dps.end());
		size_t numMinRequiredDataPoints = numInitialDataPoints
				* (1.0f - curveFitParms.maxAcceptedOutliersPerc / 100.0f);

		bool wantOutlierRemoval = (curveFitParms.outlierBoundaryFactor > 0);
		const size_t numMinInitialRequiredDataPoints = 8; // TODO: ok? Probably more... 10 ?

		// Check input parameters
		THROW_IF(CurveFit,
				numInitialDataPoints < numMinInitialRequiredDataPoints,
				"Insufficient data points.");
		THROW_IF(CurveFit, curveFitParms.numMaxIterations < 1,
				"Number of iterations needs to be at least 1.");
		THROW_IF(CurveFit, curveFitParms.epsrel < 0,
				"Relative error needs to be greater or equal 0.");
		THROW_IF(CurveFit, curveFitParms.epsabs < 0,
				"Absolute error needs to be greater or equal 0.");
		THROW_IF(CurveFit,
				curveFitParms.maxAcceptedOutliersPerc < 0
						|| curveFitParms.maxAcceptedOutliersPerc > 100,
				"Max accepted outlier percentage out of range 0..100.");

		// Take an initial copy of the incoming data points.
		//std::vector<float> dataPointsToFit;
		std::vector<PointFT> dataPointsToFit;
		boost::range::copy(dps, std::back_inserter(dataPointsToFit));
		bool continueMatching;
		
		do {
			std::vector<PointFT> remainingDps;
			std::vector<PointWithResidualT> newOutliers;
			LmCurveMatcherSummaryT lmCurveMatcherSummary;

			LOG(debug) << "CurveFitAlgorithmT::fitCurve() - dataPointsToFit.size(): " << dataPointsToFit.size() << "...(1)..."<< std::endl;
			
			try {
				// Do the curve fit
				cp = cm.fit(dataPointsToFit, curveFitParms,
						&lmCurveMatcherSummary);
				
				LOG(debug) << "CurveFitAlgorithmT::fitCurve() - dataPointsToFit.size(): " << dataPointsToFit.size() << "...(2)..."<< std::endl;

				// Process fit summary
				summary.numIterationsRequiredTotal +=
						lmCurveMatcherSummary.numIterationsRequired;

				// Only continue if outlier detections is enabled
				if (!wantOutlierRemoval) {
					break;
				}

				// Calc residuals (lambda function) (currently hardcoded here)
				auto residualFunction =
						[&](const PointFT & p) {return std::fabs(p.y() - fc->fx(p.x(), cp));};

				auto pointsWithResiduals =
						dataPointsToFit
								| boost::adaptors::transformed(
											       [&](const auto & p) {return PointWithResidualT(p, residualFunction(p)); });
				
				// Calc outlier boundary (only the first time)
				if (!outlierFilter.isSet()) {
					auto residuals =
							pointsWithResiduals
									| boost::adaptors::transformed(
											[&](const auto & pr) {return pr.residual;});

					float outlierBoundary = curveFitParms.outlierBoundaryFactor
							* QuantileT::calc<float>(residuals, 0.75) /*q3*/;

					outlierFilter.setOutlierBoundary(outlierBoundary);

					summary.outlierBoundary = outlierBoundary;
				}

				LOG(debug) << "CurveFitAlgorithmT::fitCurve() - dataPointsToFit.size(): " << dataPointsToFit.size() << "...(3)..."<< std::endl;

				// Filter outliers away
				dataPointsToFit = outlierFilter.filter(pointsWithResiduals); // returns std::vector<PointFT>, destination dataPointsToFit [std::vector<PointFT>]

				LOG(debug) << "CurveFitAlgorithmT::fitCurve() - dataPointsToFit.size(): " << dataPointsToFit.size() << "...(4)..."<< std::endl;

				//std::vector<PointFT> dataPointsToFitNew; 
				//boost::push_back(dataPointsToFitNew, rng | boost::adaptors::replaced_if(pred, new_value));
				
				newOutliers = outlierFilter.getOutliers();

				// Add new outliers
				summary.outliers.insert(std::end(summary.outliers),
						std::begin(newOutliers), std::end(newOutliers));

			} catch (LmCurveMatcherExceptionT & exc) {
			  std::stringstream ss;
			  ss << "Unable to fit " << FittingCurveTypeT::asStr(ct)
			     << " curve to datapoints. " << exc.what();
			  
			  throw CurveFitExceptionT(ss.str(), summary);
			}

			// Check if we are finished, have to continue or if curve determination failed
			continueMatching = !newOutliers.empty();
			bool tooManyOutliers = (dataPointsToFit.size()
					< numMinRequiredDataPoints);
			bool failed = tooManyOutliers;

			if (failed) {
				throw CurveFitExceptionT(
						"Unable to fit curve to datapoints. Too many outliers.",
						summary);
			}

		} while (continueMatching);

		// Finally, and only in case of success take a copy of the matched data points (the ones
		// which correspond to the curve parms returned). In case the matching fails, this container
		// is empty.
		//summary.matchedDataPoints = dataPointsToFit;
		// TODO: Should we call summary.matchedDataPoints.clear() first?
		//std::copy(dataPointsToFit.begin(), dataPointsToFit.end(), std::back_inserter(summary.matchedDataPoints));
		boost::range::copy(dataPointsToFit, std::back_inserter(summary.matchedDataPoints));
		
		//summary.curveDataPoints = dataPointsToFit | boost::adaptors::transformed([&](const auto & p) { return PointFT(p.x(), fc->fx(p.x(), cp)); })		// TODO: Should we call summary.curveDataPoints.clear() first?;
		boost::range::copy(dataPointsToFit | boost::adaptors::transformed([&](const auto & p) { return PointFT(p.x(), fc->fx(p.x(), cp)); }), std::back_inserter(summary.curveDataPoints));


		
		if (outSummary != nullptr) {
			*outSummary = summary;
		}

		// success
		return cp;
	}

};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FIT_ALGORITHM_H_*/
