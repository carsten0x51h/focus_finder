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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_POINT_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_POINT_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_POINT_H_

#include <tuple>

#include "tuple_printer.h"
#include <limits>
#include <cmath>

template<typename T> class PointT;  // pre-declare the template class itself
template<typename T> std::ostream& operator<< (std::ostream& os, const PointT <T>& p);

/**
 * PointT structure (X x Y).
 */
template<class T>
class PointT {
public:
    PointT() :mX(0), mY(0) {}

    PointT(const T &x, const T &y) : mX(x), mY(y) {}

    [[nodiscard]] const T &x() const { return mX; }

    void setX(const T &x) { mX = x; }

    [[nodiscard]] const T &y() const { return mY; }

    void setY(const T &y) { mY = y; }

    template<class U>
    PointT<U> to() const { return PointT<U>(x(), y()); }

private:
    T mX;
    T mY;
};


template<class T>
std::ostream& operator<<(std::ostream& os, const PointT<T>& p)
{
    os << "(" << p.x() << "," << p.y() << ")";
    return os;
}

template<class T>
bool operator==(const PointT<T>& p1, const PointT<T>& p2)
{
    return (p1.x() == p2.x() && p1.y() == p2.y());
}

template<class T>
bool operator!=(const PointT<T>& p1, const PointT<T>& p2)
{
    // Use implementation of operator== and negate it.
    return !(p1 == p2);
}

template<>
bool operator==<float>(const PointT<float>& p1, const PointT<float>& p2);

template<>
bool operator==<double>(const PointT<double>& p1, const PointT<double>& p2);


/**
 * Definition: One point is "bigger" than another when the distance to (0,0) is bigger.
 * NOTE: This might have a performance impact and may have to be changed...
 *
 * @tparam T
 * @param p1
 * @param p2
 * @return
 *
 * NOTE: It is not checked if the pow() overshoots the limit of the respective data type.
 */
template<class T>
bool operator<(const PointT<T>& p1, const PointT<T>& p2)
{
 // An idiomatic way to implement strict weak ordering for a structure is to use lexicographical comparison provided by std::tie.
 // See https://en.cppreference.com/w/cpp/language/operators
 return (std::tie(p1.x(), p1.y()) < std::tie(p2.x(), p2.y()));
}


using PointFT = PointT<float>;

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_POINT_H_ */
