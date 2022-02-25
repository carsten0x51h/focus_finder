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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LINEAR_BW_STRETCH_MAPPER_FUNCTION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LINEAR_BW_STRETCH_MAPPER_FUNCTION_H_

#include <string>

#include "mapper_function.h"

class LinearBWStretchMapperFunctionT : public MapperFunctionT {
public:
    LinearBWStretchMapperFunctionT();

    virtual ~LinearBWStretchMapperFunctionT();

    [[nodiscard]] std::string getName() const override;

    // TODO: Maybe move to parent class
    void setSrcRange(float from, float to) override;

    // TODO: Maybe move to parent class
    void setDestRange(float from, float to) override;

    float f(float f) override;

    void setBlackPoint(float blackPoint);

    void setWhitePoint(float whitePoint);

private:
    float mBlackPoint;
    float mWhitePoint;

    // TODO: Maybe move to parent class
    float mSrcFrom;
    float mSrcTo;
    float mDestFrom;
    float mDestTo;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LINEAR_BW_STRETCH_MAPPER_FUNCTION_H_*/
