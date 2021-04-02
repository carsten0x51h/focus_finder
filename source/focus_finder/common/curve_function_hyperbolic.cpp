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

#include "include/curve_function_hyperbolic.h"
#include "include/curve_parms.h"
#include "include/math_functions.h"
#include "include/point.h"

CurveFunctionHyperbolicT::CurveFunctionHyperbolicT(const CurveParmsT &curveParms) : CurveFunctionT(curveParms) {

}

std::string CurveFunctionHyperbolicT::getName() const {
    return "CurveFunctionHyperbolicT";
}

double CurveFunctionHyperbolicT::f(float x) const {

    float a = mCurveParms.get(IdxT::A_IDX).getValue();
    float b = mCurveParms.get(IdxT::B_IDX).getValue();
    float c = mCurveParms.get(IdxT::C_IDX).getValue();
    float d = mCurveParms.get(IdxT::D_IDX).getValue();

    return MathFunctionsT::hyperbolic(x, a, b, c, d);
}


float CurveFunctionHyperbolicT::f_inv(float x) const {
    float a = mCurveParms.get(IdxT::A_IDX).getValue();
    float b = mCurveParms.get(IdxT::B_IDX).getValue();
    float c = mCurveParms.get(IdxT::C_IDX).getValue();
    float d = mCurveParms.get(IdxT::D_IDX).getValue();

    return (float) MathFunctionsT::hyperbolic_inv(x, a, b, c, d);
}


PointFT CurveFunctionHyperbolicT::min() const {
    // For the hyperbolic curve, "c" is the minimum x value.
    // TODO: Is this always true? Can't it be the max. as well?
    float x = mCurveParms.get(IdxT::C_IDX).getValue();
    return PointFT(x, f(x));
}

PointFT CurveFunctionHyperbolicT::max() const {
    // TODO: Implement...?
    throw CurveFunctionExceptionT("CurveFunctionHyperbolicT::max() not implemented.");
}

