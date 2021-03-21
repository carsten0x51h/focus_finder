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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_H_

#include "curve_function.h"
#include "curve_parms.h"
#include "point.h"
#include "exception.h"

DEF_Exception(CurveFunction);

class CurveFunctionT {
protected:
    CurveParmsT mCurveParms;

public:
    CurveFunctionT(const CurveParmsT &curveParms);

    const CurveParmsT &getCurveParms() const;

    virtual std::string getName() const = 0;

    virtual float f(float x) const = 0;

    virtual float f_inv(float x) const = 0;

    virtual PointFT min() const = 0; // TODO: Is it a good idea to have a min() function in a generic CurveFunctionT?
    virtual PointFT max() const = 0; // TODO: Is it a good idea to have a max() function in a generic CurveFunctionT?
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_H_*/
