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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_RECT_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_RECT_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_RECT_H_

#include <tuple>

#include "point.h"
#include "size.h"
#include "logging.h"
#include "exception.h"
#include "tuple_printer.h"

DEF_Exception(Rect);

/**
 * RectT structure (X x Y x W x H).
 */
template<class T>
class RectT : public std::tuple<T, T, T, T> {
public:
    RectT() :
            std::tuple<T, T, T, T>(), mIsSet(false) {
    }

    RectT(const T &x, const T &y, const T &w, const T &h) :
            std::tuple<T, T, T, T>(x, y, w, h), mIsSet(true) {
    }

    template<class U>
    RectT<U> to() const { return RectT<U>(x(), y(), width(), height()); }

    const T &x() const {
        return std::get<0>(*this);
    }

    void setX(const T &x) {
        mIsSet = true;
        std::get<0>(*this) = x;
    }

    const T &y() const {
        return std::get<1>(*this);
    }

    void setY(const T &y) {
        mIsSet = true;
        std::get<1>(*this) = y;
    }

    const T &width() const {
        return std::get<2>(*this);
    }

    void setWidth(const T &width) {
        mIsSet = true;
        std::get<2>(*this) = width;
    }

    const T &height() const {
        return std::get<3>(*this);
    }

    void setHeight(const T &height) {
        mIsSet = true;
        std::get<3>(*this) = height;
    }

    [[nodiscard]] bool isSet() const {
        return mIsSet;
    }

    void clear() {
        mIsSet = false;
        *this = {};
    } // TODO:  {} ok??

    // Is passed rect completely contained inside this rect?
    // Of course both coordinates need to be in the same coordinate system.
    bool contains(const RectT<T> &rect) {
        return rect1ContainsRect2(*this, rect);
    }

//	TODO: operator= to allow assignment from Rect<float> -> Rect<unsigned int> etc. ??

    // Is this rect completely inside the passed rect?
    // Of course both coordinates need to be in the same coordinate system.
    bool inside(const RectT<T> &rect) {
        return rect1ContainsRect2(rect, *this);
    }

    static RectT<T> fromCenterPoint(const PointT<T> &inCenter, T inWidth, T inHeight) {
        // TODO: Or should we round here to int? Check if -1 is correct...
        //unsigned int halfWindowWidth = ceil(inWidth / 2.0f) - 1;
        //unsigned int  halfWindowHeight = ceil(inHeight / 2.0f) - 1;

        // TODO: Does not work in case of float!!!
        //            -> Implement for RectT and not for RectF... or tmpl specialization!
        //            -> Easiest: Only permit odd value!
        if (isEven(inWidth) || isEven(inHeight)) {
            // TODO: throw!
            throw RectExceptionT("Window width and height expected to be odd values.");
        }

        T deltaW = (inWidth - 1) / 2;
        T deltaH = (inWidth - 1) / 2;

//	  T halfWindowWidth = inWidth / 2.0f;
//	  T halfWindowHeight = inHeight / 2.0f;
//	  return RectT<T>(std::get<0>(inCenter) /*cx*/ - halfWindowWidth, std::get<1>(inCenter) /*cy*/ - halfWindowHeight, inWidth, inHeight);

        return RectT<T>(inCenter.x() - deltaW,
                        inCenter.y() - deltaH,
                        inWidth,
                        inHeight);
    }

    static RectT<T>
    fromCenterPoint(const PointT<T> &inCenter, SizeT<T> inWindowSize) {
        return fromCenterPoint(inCenter, inWindowSize.width(), inWindowSize.height());
    }

    static RectT<T>
    fromCenterPoint(const PointT<T> &inCenter, T inWindowSizeSquareEdge) {
        return fromCenterPoint(inCenter, inWindowSizeSquareEdge, inWindowSizeSquareEdge);
    }

private:
    // TODO: Does not work in case of float!!!
    static bool isEven(int n) {
        return (n % 2 == 0);
    }

    // Is rect1 completely contained inside this rect2?
    // Of course both coordinates need to be in the same coordinate system.
    static bool rect1ContainsRect2(const RectT<T> &rect1, const RectT<T> &rect2) {

        LOG(debug) << "rect1ContainsRect2 - rect1=" << rect1 << ", rect2=" << rect2 << std::endl;

        if (!rect1.mIsSet || !rect2.mIsSet) {
            return false;
        }

        bool contains = rect2.x() >= rect1.x() &&
                        rect2.y() >= rect1.y() &&
                        rect2.x() + rect2.width() <= rect1.x() + rect1.width() &&
                        rect2.y() + rect2.height() <= rect1.y() + rect1.height();

        return contains;
    }

    bool mIsSet;
};

// TODO?
//using RectFT = RectTmplT<float>;
//using RectT = RectTmplT<int>;

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_RECT_H_ */
