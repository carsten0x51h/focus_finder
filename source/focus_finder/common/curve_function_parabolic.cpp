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

#include "include/curve_function_parabolic.h"
#include "include/curve_parms.h"
#include "include/math_functions.h"
#include "include/point.h"

CurveFunctionParabolicT::CurveFunctionParabolicT(const CurveParmsT &curveParms) : CurveFunctionT(curveParms) {

}
CurveFunctionParabolicT::~CurveFunctionParabolicT() = default;

std::string CurveFunctionParabolicT::getName() const {
    return "CurveFunctionParabolicT";
}

double CurveFunctionParabolicT::f(float x) const {

    float a = mCurveParms.get(IdxT::A_IDX).getValue();
    float b = mCurveParms.get(IdxT::B_IDX).getValue();
    float c = mCurveParms.get(IdxT::C_IDX).getValue();

    return MathFunctionsT::parabolic(x, a, b, c);
}

float CurveFunctionParabolicT::f_inv(float /*x*/) const {
    // TODO: IMPLEMENT
    throw CurveFunctionExceptionT("CurveFunctionParabolicT::f_inv() not implemented.");
}

PointFT CurveFunctionParabolicT::min() const {
    // TODO: Implement...

    // NOTE: Parabel has 3 cases:
    //   - a > 0: min
    //   - a < 0: max
    //   - a = 0: none, no real parabel
    //
    // -> Function needs to be converted to "Scheitelpunktsform"! -> then d and e directly give the (x,y) pos of the extremum.
    throw CurveFunctionExceptionT("CurveFunctionParabolicT::min() not implemented.");
}

PointFT CurveFunctionParabolicT::max() const {
    // TODO: Implement...
    throw CurveFunctionExceptionT("CurveFunctionParabolicT::max() not implemented.");
}
