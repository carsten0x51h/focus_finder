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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SIZE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SIZE_H_

#include <tuple>

#include "tuple_printer.h"


/**
 * SizeT structure (W x H).
 */
template<class T>
class SizeT : public std::tuple<T, T> {
public:
    SizeT() : std::tuple<T, T>() {}

    SizeT(const T &w, const T &h) : std::tuple<T, T>(w, h) {}

    const T &width() const { return std::get<0>(*this); }

    void setWidth(const T &w) { std::get<0>(*this) = w; }

    const T &height() const { return std::get<1>(*this); }

    void setHeight(const T &h) { std::get<1>(*this) = h; }

    template<class U>
    SizeT<U> to() const { return SizeT<U>(width(), height()); }

    SizeT<T> operator*(T factor) { return SizeT<T>(width() * factor, height() * factor); }
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SIZE_H_ */

