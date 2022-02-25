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

#include "include/self_orientation_result.h"
#include "include/focus_curve_record.h"

std::ostream &
SelfOrientationResultT::print(std::ostream &os) const {
    os << "--- SelfOrientationResultT ---" << std::endl
       << "Focus direction to closest limit: " << FocusDirectionT::asStr(focusDirectionToLimit) << std::endl
       << "We are in curve half: " << CurveSectorT::asStr(curveHalf) << std::endl
       << "Record1: " << std::endl
       << *record1 << std::endl
       << "Record2: " << std::endl
       << *record2;

    return os;
}

std::ostream &operator<<(std::ostream &os, const SelfOrientationResultT &selfOrientationResult) {
    return selfOrientationResult.print(os);
}
