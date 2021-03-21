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

#include <vector>

#include "include/logging.h"
#include "include/throw_if.h"
#include "include/lm_fitting_curve_parabolic.h"
#include "include/lm_curve_matcher.h"
#include "include/math_functions.h"

std::string LmFittingCurveParabolicT::getName() const {
    return "LmFittingCurveParabolicT";
}

std::vector<std::string>
LmFittingCurveParabolicT::getParmNames() const { return LmFittingCurveT::getParmNames<IdxT>(); }

/**
 * Makes a guess for a, b and c based on the supplied data.
 */
void LmFittingCurveParabolicT::makeGuess(const GslMultiFitParmsT &inData,
                                         gsl_vector *guess) {
    THROW_IF(LmCurveMatcher, inData.size() < 2, "inData.size() < 2!");

    float xMin = inData.at(0).pt.x();
    float yMin = inData.at(0).pt.y();

    // Find minimum y value...
    for (size_t i = 0; i < inData.size(); ++i) {
        const PointFT &dataPoint = inData.at(i).pt;
        if (dataPoint.y() < yMin) {
            xMin = dataPoint.x();
            yMin = dataPoint.y();
        }
    }

    LOG(debug) << "xMin: " << xMin << ", yMin: " << yMin << std::endl;

    // Guessing of parameters a,b and c using prominent coordinates of the curve
    const float x1 = inData.at(0).pt.x(); // Left value
    const float y1 = inData.at(0).pt.y();
    const float x2 = xMin;
    const float y2 = yMin;
    const float x3 = inData.back().pt.x(); // Right value
    const float y3 = inData.back().pt.y();

    const float x1x2sq = x1 * x1 - x2 * x2;
    const float x1x3sq = x1 * x1 - x3 * x3;

    const float bNumerator = (y1 - y2) * x1x3sq - y1 * (x1 * x1 - x2 * x2)
                             + y3 * x1x2sq;
    const float bDenominator = (x1 - x3) * x1x2sq - (x1 - x2) * x1x3sq;

    const float bGuess = -bNumerator / bDenominator;
    const float aGuess = (y1 - y2 - bGuess * (x1 - x2)) / x1x2sq;
    const float cGuess = y2 - aGuess * x2 * x2 - bGuess * x2;

    LOG(debug) << "Guessing a=" << aGuess << ", b=" << bGuess << ", c="
               << cGuess << std::endl;

    gsl_vector_set(guess, IdxT::A_IDX, aGuess);
    gsl_vector_set(guess, IdxT::B_IDX, bGuess);
    gsl_vector_set(guess, IdxT::C_IDX, cGuess);
}

/* y = a * x^2 + b * x + c */
float LmFittingCurveParabolicT::fx(float x, const gsl_vector *curveParms) const {

    float a = gsl_vector_get(curveParms, IdxT::A_IDX);
    float b = gsl_vector_get(curveParms, IdxT::B_IDX);
    float c = gsl_vector_get(curveParms, IdxT::C_IDX);

    return MathFunctionsT::parabolic(x, a, b, c);
}

/* Calculates f(x) = a*x^2 + b*x + c for each data point */
int LmFittingCurveParabolicT::gslFx(const gsl_vector *curveParms, const GslMultiFitParmsT *gslParms,
                                    gsl_vector *outResultVec) {

    /* Execute Levenberg-Marquart on f(x) */
    for (size_t i = 0; i < gslParms->size(); ++i) {
        const GslMultiFitDataT &gslData = gslParms->at(i);
        float yi = fx(gslData.pt.x(), curveParms);

        gsl_vector_set(outResultVec, i, (yi - gslData.y) / gslData.sigma);
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
int LmFittingCurveParabolicT::gslDfx(const gsl_vector *x, const GslMultiFitParmsT *gslParms, gsl_matrix *J) {
    /* Store current coefficients */
    // float a = gsl_vector_get(x, IdxT::A_IDX);
    // float b = gsl_vector_get(x, IdxT::B_IDX);
    // float c = gsl_vector_get(x, IdxT::C_IDX);

    for (size_t i = 0; i < gslParms->size(); ++i) {
        const GslMultiFitDataT &gslData = gslParms->at(i);
        const float oneBySigma = 1.0F / gslData.sigma;
        const float x = gslData.pt.x();

        gsl_matrix_set(J, i, IdxT::A_IDX, oneBySigma * x * x);
        gsl_matrix_set(J, i, IdxT::B_IDX, oneBySigma * x);
        gsl_matrix_set(J, i, IdxT::C_IDX, oneBySigma);
    }

    return GSL_SUCCESS;
}

/* Invokes f(x) and f'(x) */
int LmFittingCurveParabolicT::gslFdfx(const gsl_vector *x, const GslMultiFitParmsT *gslParms, gsl_vector *f,
                                      gsl_matrix *J) {
    gslFx(x, gslParms, f);
    gslDfx(x, gslParms, J);

    return GSL_SUCCESS;
}
