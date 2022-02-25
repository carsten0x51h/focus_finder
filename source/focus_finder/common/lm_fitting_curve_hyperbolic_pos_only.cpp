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

#include <vector>

// TODO: Find better name for class......

#include "include/throw_if.h"
#include "include/lm_fitting_curve_hyperbolic_pos_only.h"
#include "include/lm_curve_matcher.h"
#include "include/curve_function_hyperbolic.h"
#include "include/curve_function_hyperbolic_pos_only.h" // HACK!
#include "include/math_functions.h"

LmFittingCurveHyperbolicPosOnlyT::LmFittingCurveHyperbolicPosOnlyT() {
    mA = 15268.2;
    mB = 11.074;
}

std::string LmFittingCurveHyperbolicPosOnlyT::getName() const {
    return "LmFittingCurveHyperbolicPosOnlyT";
}

std::vector<std::string>
LmFittingCurveHyperbolicPosOnlyT::getParmNames() const { return LmFittingCurveT::getParmNames<CurveFunctionHyperbolicPosOnlyT::IdxT>(); }

/**
 * In this case we just initialize all parameters with 1.
 */
void LmFittingCurveHyperbolicPosOnlyT::makeGuess(const GslMultiFitParmsT &inData,
                                                 gsl_vector *guess) {
    THROW_IF(LmCurveMatcher, inData.size() < 2, "inData.size() < 2!")

    // Find min HFD -> good start value for c
    GslMultiFitParmsT::const_iterator minEl;
    GslMultiFitParmsT::const_iterator maxEl;

    std::tie(minEl, maxEl) = std::minmax_element(std::begin(inData),
                                                 std::end(inData),
                                                 [](GslMultiFitDataT const &s1, GslMultiFitDataT const &s2) {
                                                     return s1.pt.y() < s2.pt.y();
                                                 });

    //gsl_vector_set(guess, CurveFunctionHyperbolicPosOnlyT::IdxT::C_IDX, minEl->pt.x());
    gsl_vector_set(guess, CurveFunctionHyperbolicPosOnlyT::IdxT::C_IDX, 30000); // TODO: HACK - remove!
    gsl_vector_set(guess, CurveFunctionHyperbolicPosOnlyT::IdxT::D_IDX, 1.0);
}

/**
 * Phi() is a repeating part of the function calculation used by
 * different derivations.
 */
float LmFittingCurveHyperbolicPosOnlyT::phi(float x, float a, float c) {
    const float a2 = a * a;
    const float diff = x - c;
    const float diff2 = diff * diff;

    return (float) std::sqrt(1.0 + (diff2 / a2));
}

void LmFittingCurveHyperbolicPosOnlyT::setA(float a) {
    mA = a;
}

void LmFittingCurveHyperbolicPosOnlyT::setB(float b) {
    mB = b;
}


/* Calculate H(x) */
float LmFittingCurveHyperbolicPosOnlyT::fx(float x, const gsl_vector *curveParms) const {

    auto c = (float) gsl_vector_get(curveParms, CurveFunctionHyperbolicPosOnlyT::IdxT::C_IDX);
    auto d = (float) gsl_vector_get(curveParms, CurveFunctionHyperbolicPosOnlyT::IdxT::D_IDX);

    return (float) MathFunctionsT::hyperbolic(x, mA, mB, c, d);
}

/* Calculates fitting function for H(x) for each data point. */
int LmFittingCurveHyperbolicPosOnlyT::gslFx(const gsl_vector *curveParms,
                                            const GslMultiFitParmsT *gslParms, gsl_vector *outResultVec) {

    // Execute Levenberg-Marquart on f(x)
    for (size_t i = 0; i < gslParms->size(); ++i) {
        const GslMultiFitDataT &gslData = gslParms->at(i);
        float yi = fx(gslData.pt.x(), curveParms);

        // TODO: Is this ok?
        gsl_vector_set(outResultVec, i, (yi - gslData.y) / gslData.sigma);
    }

    return GSL_SUCCESS;
}

/* Calculates the Jacobian (derivative) matrix  */
int LmFittingCurveHyperbolicPosOnlyT::gslDfx(const gsl_vector *xv, const GslMultiFitParmsT *gslParms,
                                             gsl_matrix *J) {

    // Store current coefficients
    auto c = (float) gsl_vector_get(xv, CurveFunctionHyperbolicPosOnlyT::IdxT::C_IDX);

    // Store non-changing calculations
    const float a2 = mA * mA;

    for (size_t i = 0; i < gslParms->size(); ++i) {
        const GslMultiFitDataT &gslData = gslParms->at(i);

        const float oneBySigma = 1.0F / gslData.sigma;
        const float x = gslData.pt.x();
        const float x_minus_c = x - c;

        gsl_matrix_set(J, i, CurveFunctionHyperbolicPosOnlyT::IdxT::C_IDX,
                       -oneBySigma * mB * x_minus_c / (a2 * phi(x, mA, c)));
        gsl_matrix_set(J, i, CurveFunctionHyperbolicPosOnlyT::IdxT::D_IDX, oneBySigma);
    }

    return GSL_SUCCESS;
}

/* Invokes f(x) and f'(x) */
int LmFittingCurveHyperbolicPosOnlyT::gslFdfx(const gsl_vector *x, const GslMultiFitParmsT *gslParms,
                                              gsl_vector *f, gsl_matrix *J) {
    gslFx(x, gslParms, f);
    gslDfx(x, gslParms, J);

    return GSL_SUCCESS;
}
