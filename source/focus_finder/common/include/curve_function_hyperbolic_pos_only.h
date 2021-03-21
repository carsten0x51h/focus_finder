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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_HYPERBOLIC_POS_ONLY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_HYPERBOLIC_POS_ONLY_H_

#include "curve_function.h"
#include "curve_parms.h"
#include "enum_helper.h"
#include "point.h"

class CurveFunctionHyperbolicPosOnlyT : public CurveFunctionT {
public:
    struct IdxT {
        enum TypeE {
            C_IDX = 0, D_IDX, _Count
        };

        static const char *asStr(const TypeE &inType) {
            switch (inType) {
                case C_IDX:
                    return "C_IDX";
                case D_IDX:
                    return "D_IDX";
                default:
                    return "<?>";
            }
        }

        MAC_AS_TYPE(Type, E, _Count);
    };

    CurveFunctionHyperbolicPosOnlyT(const CurveParmsT &curveParms);

    std::string getName() const;

    float f(float x) const;

    float f_inv(float x) const;

    PointFT min() const;

    PointFT max() const;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_HYPERBOLIC_POS_ONLY_H_*/
