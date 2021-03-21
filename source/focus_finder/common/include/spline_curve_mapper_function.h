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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SPLINE_CURVE_MAPPER_FUNCTION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SPLINE_CURVE_MAPPER_FUNCTION_H_

#include <string>

#include "mapper_function.h"

class SplineCurveMapperFunctionT : public MapperFunctionT {
public:
    SplineCurveMapperFunctionT();

    virtual ~SplineCurveMapperFunctionT();

    std::string getName() const;

    // TODO: Maybe move to parent class
    void setSrcRange(float from, float to);

    // TODO: Maybe move to parent class
    void setDestRange(float from, float to);

    float f(float f);

private:

    // TODO: Maybe move to parent class
    float mSrcFrom;
    float mSrcTo;
    float mDestFrom;
    float mDestTo;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SPLINE_CURVE_MAPPER_FUNCTION_H_*/
