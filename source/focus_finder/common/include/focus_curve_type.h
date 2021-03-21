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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_TYPE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_TYPE_H_

#include <exception>
#include <sstream>

#include "enum_helper.h"
#include "fitting_curve_type.h"

struct FocusCurveTypeT {
    enum TypeE {
        HYPERBOLIC, PARABOLIC, HYPERBOLIC_POS_ONLY, _Count
    };

    static const char *asStr(const TypeE &inType) {
        switch (inType) {
            case HYPERBOLIC:
                return "HYPERBOLIC";
            case PARABOLIC:
                return "PARABOLIC";
            case HYPERBOLIC_POS_ONLY:
                return "HYPERBOLIC_POS_ONLY";
            default:
                return "<?>";
        }
    }

    MAC_AS_TYPE(Type, E, _Count);

    static FittingCurveTypeT::TypeE toFittingCurve(FocusCurveTypeT::TypeE focusCurveType) {
        return FittingCurveTypeT::asType(FocusCurveTypeT::asStr(focusCurveType));
    }

    static FocusCurveTypeT::TypeE fromFittingCurve(FittingCurveTypeT::TypeE fittingCurveType) {
        FocusCurveTypeT::TypeE focusCurveType = FocusCurveTypeT::asType(FittingCurveTypeT::asStr(fittingCurveType));

        if (focusCurveType == FocusCurveTypeT::_Count) {
            // TODO: If bad_cast exception works, create
            std::stringstream ss;
            ss << "Cannot convert FittingCurveTypeT '" << FittingCurveTypeT::asStr(fittingCurveType) << "'"
               << " to FocusCurveTypeT. Type not supported." << std::endl;

            throw std::invalid_argument(ss.str());
        }

        return focusCurveType;
    }
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_TYPE_H_ */

