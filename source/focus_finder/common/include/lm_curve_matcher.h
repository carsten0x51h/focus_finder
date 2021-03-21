/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c.schmitt51h@gmail.com>
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
    CurveParmsT fit(const RngT &rng, const CurveFitParmsT &curveFitParms,
                    LmCurveMatcherSummaryT *outLmCurveMatcherSummary = nullptr) {

        CurveParmsT curveParms;
        LmCurveMatcherSummaryT lmCurveMatcherSummary;
        size_t numPoints = std::distance(rng.begin(), rng.end());

        // TODO: FIXED_PARMS - Only return number of non-fixed parms... ask mLmFittingCurve for this... (generic IF)...
        size_t numCurveParams = mLmFittingCurve->getParmNames().size();
        LOG(debug) << "LmCurveMatcher::fit() - numPoints: " << numPoints << std::endl;

        GslMultiFitParmsT gslMultiFitParms;
        LmFittingParmsContainerT parmsContainer(&gslMultiFitParms, mLmFittingCurve.get());

        // Create a service data structure for each data point
        for (auto it = rng.begin(); it != rng.end(); ++it) {
            gslMultiFitParms.push_back(GslMultiFitDataT(it->y() /*y*/, 0.1 /*sigma*/, *it /*data point*/));
        }

        // Fill in function info
        // See http://www.csse.uwa.edu.au/programming/gsl-1.0/gsl-ref_35.html
        gsl_multifit_function_fdf f;
        f.f = &LmFittingCurveT::gslFx;
        f.df = &LmFittingCurveT::gslDfx;
        f.fdf = &LmFittingCurveT::gslFdfx;
        f.n = numPoints;
        f.p = numCurveParams;
        f.params = &parmsContainer;

        gsl_vector *guess = gsl_vector_alloc(numCurveParams); // Allocate the guess vector

        //TODO: FIXED_PARMS - gslFx, gslDfx and gslFdfx must also take into account only the non-fixed params! makeGuess() as well!

        mLmFittingCurve->makeGuess(gslMultiFitParms, guess); // Make initial guesses

        // Create a Levenberg-Marquardt solver with n data points and m parameters
        gsl_multifit_fdfsolver *solver = gsl_multifit_fdfsolver_alloc(
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
            status = gsl_multifit_test_delta(solver->dx, solver->x, curveFitParms.getMaxAcceptedAbsoluteError(),
                                             curveFitParms.getMaxAcceptedRelativError());

        } while (GSL_CONTINUE == status && iter < curveFitParms.getNumMaxIterations());

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
    CurveParmsT fillCurveParms(const gsl_vector *resultVec) const;

    std::shared_ptr<LmFittingCurveT> mLmFittingCurve;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_CURVE_MATCHER_H_*/
