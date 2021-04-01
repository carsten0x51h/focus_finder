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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_PARMS_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_PARMS_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_PARMS_H_

#include <vector>

#include "exception.h"
#include "curve_parm.h"

/**
 * Usage:
 *
 * myCurveParms.get(IDX_B).getValue();
 * myCurveParms.get(IDX_B).getName();
 */
class CurveParmsT {
private:
    std::vector<CurveParmT> mCurveParms;

public:
    CurveParmsT();

    explicit CurveParmsT(size_t size);

    [[nodiscard]] size_t size() const;

    [[nodiscard]] const CurveParmT &get(size_t idx) const;

    [[nodiscard]] const CurveParmT &get(const std::string &name) const;

    CurveParmT &operator[](size_t idx);

    std::ostream &print(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const CurveParmsT &curveParms);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_PARMS_H_*/
