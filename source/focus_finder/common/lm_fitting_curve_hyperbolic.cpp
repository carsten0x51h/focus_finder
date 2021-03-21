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

#include "include/throw_if.h"
#include "include/lm_fitting_curve_hyperbolic.h"
#include "include/lm_curve_matcher.h"
#include "include/curve_function_hyperbolic.h"
#include "include/math_functions.h"


std::string LmFittingCurveHyperbolicT::getName() const {
    return "LmFittingCurveHyperbolicT";
}

std::vector<std::string>
LmFittingCurveHyperbolicT::getParmNames() const { return LmFittingCurveT::getParmNames<CurveFunctionHyperbolicT::IdxT>(); }

/**
 * In this case we just initialize all parameters with 1.
 */
void LmFittingCurveHyperbolicT::makeGuess(const GslMultiFitParmsT &inData,
                                          gsl_vector *guess) {
    THROW_IF(LmCurveMatcher, inData.size() < 2, "inData.size() < 2!");

    // Find min HFD -> good start value for c
    GslMultiFitParmsT::const_iterator minEl;
    GslMultiFitParmsT::const_iterator maxEl;

    std::tie(minEl, maxEl) = std::minmax_element(std::begin(inData),
                                                 std::end(inData),
                                                 [](GslMultiFitDataT const &s1, GslMultiFitDataT const &s2) {
                                                     return s1.pt.y() < s2.pt.y();
                                                 });

    gsl_vector_set(guess, CurveFunctionHyperbolicT::IdxT::A_IDX, 1.0);
    gsl_vector_set(guess, CurveFunctionHyperbolicT::IdxT::B_IDX, 2.0);
    gsl_vector_set(guess, CurveFunctionHyperbolicT::IdxT::C_IDX, minEl->pt.x());
    gsl_vector_set(guess, CurveFunctionHyperbolicT::IdxT::D_IDX, 1.0);
}

/**
 * Phi() is a repeating part of the function calculation used by
 * different derivations.
 */
float LmFittingCurveHyperbolicT::phi(float x, float a, float c) const {
    const float a2 = a * a;
    const float diff = x - c;
    const float diff2 = diff * diff;

    return sqrt(1.0 + (diff2 / a2));
}

/* Calculate H(x) */
float LmFittingCurveHyperbolicT::fx(float x, const gsl_vector *curveParms) const {

    float a = gsl_vector_get(curveParms, CurveFunctionHyperbolicT::IdxT::A_IDX);
    float b = gsl_vector_get(curveParms, CurveFunctionHyperbolicT::IdxT::B_IDX);
    float c = gsl_vector_get(curveParms, CurveFunctionHyperbolicT::IdxT::C_IDX);
    float d = gsl_vector_get(curveParms, CurveFunctionHyperbolicT::IdxT::D_IDX);

    // We could write a TMPL for that... but question is if this is not
    // too slow since fx() will be called very often by the LM algorithm.
    // CurveParmsT parms(curveParms.size());
    // for (size_t i = 0; i < CurveFunctionHyperbolicT::IdxT::_Count; ++i) {
    //   CurveFunctionHyperbolicT::IdxT::TypeE idx = static_cast<CurveFunctionHyperbolicT::IdxT::TypeE>(i);
    //   std::string parmName = CurveFunctionHyperbolicT::IdxT::asStr(idx);
    //   float value = gsl_vector_get(curveParms, idx);

    //   parms[idx] = CurveParmT(parmName, value);
    // }

    // CurveFunctionHyperbolicT hyperbolicFunc(parms);
    // return hyperbolicFunc.f(x);

    return MathFunctionsT::hyperbolic(x, a, b, c, d);
}

/* Calculates fitting funtion for H(x) for each data point. */
int LmFittingCurveHyperbolicT::gslFx(const gsl_vector *curveParms,
                                     const GslMultiFitParmsT *gslParms, gsl_vector *outResultVec) {

    // Store parameter values
    //const GslMultiFitParmsT * gslParms = ((GslMultiFitParmsT*) parms);

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
int LmFittingCurveHyperbolicT::gslDfx(const gsl_vector *x, const GslMultiFitParmsT *gslParms,
                                      gsl_matrix *J) {

    // Store current coefficients
    float a = gsl_vector_get(x, CurveFunctionHyperbolicT::IdxT::A_IDX);
    float b = gsl_vector_get(x, CurveFunctionHyperbolicT::IdxT::B_IDX);
    float c = gsl_vector_get(x, CurveFunctionHyperbolicT::IdxT::C_IDX);

    // Store non-changing calculations
    const float a2 = a * a;
    const float a3 = a * a2;

    for (size_t i = 0; i < gslParms->size(); ++i) {
        const GslMultiFitDataT &gslData = gslParms->at(i);

        const float oneBySigma = 1.0F / gslData.sigma;
        const float x = gslData.pt.x();
        const float x_minus_c = x - c;

        gsl_matrix_set(J, i, CurveFunctionHyperbolicT::IdxT::A_IDX,
                       -oneBySigma * b * (x_minus_c * x_minus_c)
                       / (a3 * phi(x, a, c)));
        gsl_matrix_set(J, i, CurveFunctionHyperbolicT::IdxT::B_IDX, oneBySigma * phi(x, a, c));
        gsl_matrix_set(J, i, CurveFunctionHyperbolicT::IdxT::C_IDX,
                       -oneBySigma * b * x_minus_c / (a2 * phi(x, a, c)));
        gsl_matrix_set(J, i, CurveFunctionHyperbolicT::IdxT::D_IDX, oneBySigma);
    }

    return GSL_SUCCESS;
}

/* Invokes f(x) and f'(x) */
int LmFittingCurveHyperbolicT::gslFdfx(const gsl_vector *x, const GslMultiFitParmsT *gslParms,
                                       gsl_vector *f, gsl_matrix *J) {
    gslFx(x, gslParms, f);
    gslDfx(x, gslParms, J);

    return GSL_SUCCESS;
}
