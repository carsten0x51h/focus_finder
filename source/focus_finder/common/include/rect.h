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
 *
 * TODO: Put this fully under unit tests for different data types!
 * TODO: Restrict this to data types which make sense!
 * TODO / IDEA: Make w & h as unsigned for fixed point types and x & y as signed.
 * TODO: Is there any reason why this is inherited from tuple?
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

    // TODO: mIsSet should be checked in all functions. If not set, and e.g. contains() is called, an exception should be thrown.
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
    // TODO: Maybe rename to isInside()?
    bool inside(const RectT<T> &rect) {
        return rect1ContainsRect2(rect, *this);
    }


    PointT<T> center() const {
    	return calcCenterFromRectInternal(*this);
    }

    /**
     * IDEA: Alternate name: make_square
     */
    RectT<T> expand_to_square() const {
    	auto center = calcCenterFromRectInternal(*this);
    	T sideLength = std::max(std::get<2>(*this), std::get<3>(*this));

    	return fromCenterPoint(center, sideLength);
    }

    /**
     * Grow rectangle by growBy in each direction.
     * TODO: A negative value should throw an exception...
     */
    RectT<T> grow(T growBy) const {
    	return changeRectSizeInternal(*this, growBy, true /*hrow*/);
    }

    /**
     * Shrink rectangle by shrinkBy in each direction.
     * TODO: A negative value should throw an exception...
     */
    RectT<T> shrink(T shrinkBy) const {
    	return changeRectSizeInternal(*this, shrinkBy, false /*shrink*/);
    }

    // TODO: Idea: Add
    // 2. RectT::grow(3)
    // 3. RectT::shrink(3)


    static RectT<T> fromCenterPoint(const PointT<T> &inCenter, T inWidth, T inHeight) {
        // TODO: Or should we round here to int? Check if -1 is correct...
        //unsigned int halfWindowWidth = ceil(inWidth / 2.0f) - 1;
        //unsigned int  halfWindowHeight = ceil(inHeight / 2.0f) - 1;

        // TODO: Does not work in case of float!!!
        //            -> Implement for RectT and not for RectF... or tmpl specialization!
        //            -> Easiest: Only permit odd value!
//        if (isEven(inWidth) || isEven(inHeight)) {
 //           throw RectExceptionT("Window width and height expected to be odd values.");
 //       }

        float deltaW = ((float)inWidth - 1.0F) / 2.0F;
        float deltaH = ((float)inHeight - 1.0F) / 2.0F;

//	  T halfWindowWidth = inWidth / 2.0f;
//	  T halfWindowHeight = inHeight / 2.0f;
//	  return RectT<T>(std::get<0>(inCenter) /*cx*/ - halfWindowWidth, std::get<1>(inCenter) /*cy*/ - halfWindowHeight, inWidth, inHeight);

        return RectT<T>((T)((float) inCenter.x() - deltaW),
        				(T)((float) inCenter.y() - deltaH),
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
    // TODO: If increaseBy is negative, check that overall width & height cannot get negative.
    //        -> Limit values... 0? Or exception...
    static RectT<T> changeRectSizeInternal(RectT<T> rect, T changeBy, bool grow) {
    	auto center = calcCenterFromRectInternal(rect);
    	T borderBothSides = 2 * changeBy;

    	T newWidth = (grow ? std::get<2>(rect) + borderBothSides : std::get<2>(rect) - borderBothSides);
    	T newHeight = (grow ? std::get<3>(rect) + borderBothSides : std::get<3>(rect) - borderBothSides);

    	return fromCenterPoint(center, newWidth, newHeight);
    }


    static PointT<T> calcCenterFromRectInternal(const RectT<T> & rect) {
    	T xcenter = std::get<0>(rect) + std::get<2>(rect) / 2.0;
    	T ycenter = std::get<1>(rect) + std::get<3>(rect) / 2.0;

    	return PointT<T>(xcenter, ycenter);
    }

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
