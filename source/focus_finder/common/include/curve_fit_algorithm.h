/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c [at] lost-infinity.com>
 *
 *  More info on https://www.lost-infinity.com
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FIT_ALGORITHM_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FIT_ALGORITHM_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FIT_ALGORITHM_H_

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
    static CurveParmsT fitCurve(Rng dps,
                                const CurveFitParmsT &curveFitParms,
                                CurveFitSummaryT *outSummary = nullptr) {
        FittingCurveTypeT::TypeE ct = curveFitParms.getFittingCurveType();
        std::shared_ptr<LmFittingCurveT> fc = LmFittingCurveFactoryT::getInstance(ct);
        LmCurveMatcherT cm(fc);
        OutlierFilterT outlierFilter;
        CurveParmsT cp;
        CurveFitSummaryT summary;

        size_t numInitialDataPoints = std::distance(dps.begin(), dps.end());
        size_t numMinRequiredDataPoints = numInitialDataPoints
                                          * (size_t) std::floor(1.0f - curveFitParms.getMaxAcceptedOutliersPerc() / 100.0f);

        bool wantOutlierRemoval = curveFitParms.getEnableOutlierDetection();
        const size_t numMinInitialRequiredDataPoints = 8; // TODO: ok? Probably more... 10 ? --> Depends on the problem --> add as parameter...

        // Check input parameters
        THROW_IF(CurveFit,
                 numInitialDataPoints < numMinInitialRequiredDataPoints,
                 "Insufficient data points.")
        THROW_IF(CurveFit, curveFitParms.getNumMaxIterations() < 1,
                 "Number of iterations needs to be at least 1.")
        THROW_IF(CurveFit, curveFitParms.getMaxAcceptedRelativError() < 0,
                 "Relative error needs to be greater or equal 0.")
        THROW_IF(CurveFit, curveFitParms.getMaxAcceptedAbsoluteError() < 0,
                 "Absolute error needs to be greater or equal 0.")
        THROW_IF(CurveFit,
                 curveFitParms.getMaxAcceptedOutliersPerc() < 0
                 || curveFitParms.getMaxAcceptedOutliersPerc() > 100,
                 "Max accepted outlier percentage out of range 0..100.")


        // DEBUG START
        LOG(debug) << "Data points to match (#" << numInitialDataPoints << "):" << std::endl;

        for (typename Rng::const_iterator it = dps.begin(); it != dps.end(); ++it) {
            LOG(debug) << "x: " << it->x() << ", y: " << it->y() << std::endl;

        }
        // DEBUG END


        // Take an initial copy of the incoming data points.
        //std::vector<float> dataPointsToFit;
        std::vector<PointFT> dataPointsToFit;
        boost::range::copy(dps, std::back_inserter(dataPointsToFit));
        bool continueMatching;

        do {
            std::vector<PointFT> remainingDps;
            std::vector<PointWithResidualT> newOutliers;
            LmCurveMatcherSummaryT lmCurveMatcherSummary;

            try {
                // Do the curve fit
                cp = cm.fit(dataPointsToFit, curveFitParms,
                            &lmCurveMatcherSummary);

                // Process fit summary
                summary.numIterationsRequiredTotal +=
                        lmCurveMatcherSummary.numIterationsRequired;
                summary.success = lmCurveMatcherSummary.isSuccessful();

                // Only continue if outlier detections is enabled
                if (!wantOutlierRemoval) {
                    break;
                }

                // Calc residuals (lambda function) (currently hardcoded here)
                // TODO: The residual function should be passed in as parameter.
                auto residualFunction =
                        [&](const PointFT &p) { return std::fabs(p.y() - fc->fx(p.x(), cp)); };

                auto pointsWithResiduals =
                        dataPointsToFit
                        | boost::adaptors::transformed(
                                [&](const auto &p) { return PointWithResidualT(p, residualFunction(p)); });

                // Calc outlier boundary (only the first time)
                if (!outlierFilter.isSet()) {
                    auto residuals =
                            pointsWithResiduals
                            | boost::adaptors::transformed(
                                    [&](const auto &pr) { return pr.residual; });

                    float outlierBoundary = curveFitParms.getOutlierBoundaryFactor()
                                            * QuantileT::calc<float>(residuals, 0.75) /*q3*/;

                    outlierFilter.setOutlierBoundary(outlierBoundary);

                    summary.outlierBoundary = outlierBoundary;
                }

                // Filter outliers away
                dataPointsToFit = outlierFilter.filter(
                        pointsWithResiduals); // returns std::vector<PointFT>, destination dataPointsToFit [std::vector<PointFT>]

                newOutliers = outlierFilter.getOutliers();

                // Add new outliers
                summary.outliers.insert(std::end(summary.outliers),
                                        std::begin(newOutliers), std::end(newOutliers));

            } catch (LmCurveMatcherExceptionT &exc) {
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
        boost::range::copy(dataPointsToFit, std::back_inserter(summary.matchedDataPoints));

        boost::range::copy(dataPointsToFit | boost::adaptors::transformed(
                [&](const auto &p) { return PointFT(p.x(), fc->fx(p.x(), cp)); }),
                           std::back_inserter(summary.curveDataPoints));

        if (outSummary != nullptr) {
            *outSummary = summary;
        }

        // success
        return cp;
    }

};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FIT_ALGORITHM_H_*/
