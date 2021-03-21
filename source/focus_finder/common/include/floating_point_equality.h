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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FLOATING_POINT_EQUALITY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FLOATING_POINT_EQUALITY_H_

#include <limits>
#include <cmath>  /* for std::abs(double) */

// https://stackoverflow.com/questions/19837576/comparing-floating-point-number-to-zero
// see Knuth section 4.2.2 pages 217-218
template<typename T>
static bool isAlmostEqual(T x, T y) {
    return std::abs(x - y) <= std::numeric_limits<T>::epsilon() * std::abs(x);
}

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FLOATING_POINT_EQUALITY_H_*/
