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

#include "include/spline_curve_mapper_function.h"

#include "include/logging.h"
#include "include/mapper_function.h"


SplineCurveMapperFunctionT::SplineCurveMapperFunctionT() :
        mSrcFrom(0),
        mSrcTo(0),
        mDestFrom(0),
        mDestTo(0) {

}

SplineCurveMapperFunctionT::~SplineCurveMapperFunctionT() {

}

std::string SplineCurveMapperFunctionT::getName() const {
    return "SplineCurveMapperFunction";
}

// TODO: Maybe move to parent class
void SplineCurveMapperFunctionT::setSrcRange(float from, float to) {
    // TODO: Check that from <= to
    LOG(debug) << "SplineCurveMapperFunctionT::setSrcRange... [" << from << ", " << to << "]" << std::endl;

    mSrcFrom = from;
    mSrcTo = to;
}

// TODO: Maybe move to parent class
void SplineCurveMapperFunctionT::setDestRange(float from, float to) {
    // TODO: Check that from <= to
    LOG(debug) << "SplineCurveMapperFunctionT::setDestRange... [" << from << ", " << to << "]" << std::endl;

    mDestFrom = from;
    mDestTo = to;
}

float SplineCurveMapperFunctionT::f(float f) {
    return f;
}

