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

#include "include/curve_function_gaussian.h"
#include "include/curve_parms.h"
#include "include/math_functions.h"

CurveFunctionGaussianT::CurveFunctionGaussianT(const CurveParmsT &curveParms) : CurveFunctionT(curveParms) {

}

std::string CurveFunctionGaussianT::getName() const {
    return "CurveFunctionGaussianT";
}

float CurveFunctionGaussianT::f(float x) const {

    float b = mCurveParms.get(IdxT::B_IDX).getValue();
    float p = mCurveParms.get(IdxT::P_IDX).getValue();
    float c = mCurveParms.get(IdxT::C_IDX).getValue();
    float w = mCurveParms.get(IdxT::W_IDX).getValue();

    return MathFunctionsT::gaussian(x, b, p, c, w);
}

float CurveFunctionGaussianT::f_inv(float x) const {
    // TODO: IMPLEMENT
    throw CurveFunctionExceptionT("CurveFunctionGaussianT::f_inv() not implemented.");
}

PointFT CurveFunctionGaussianT::min() const {
    // TODO: Implement...?
    throw CurveFunctionExceptionT("CurveFunctionGaussianT::min() not implemented.");
}

PointFT CurveFunctionGaussianT::max() const {
    // TODO: Implement...?
    throw CurveFunctionExceptionT("CurveFunctionGaussianT::max() not implemented.");
}
