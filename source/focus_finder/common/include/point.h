/*
 * focus.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_POINT_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_POINT_H_

#include <tuple>

#include "tuple_printer.h"

/**
 * PointT structure (X x Y).
 */
template <class T>
class PointT : public std::tuple<T,T> {
public:
	PointT() : std::tuple<T,T>() {}
	PointT(const T & x, const T & y) : std::tuple<T,T>(x,y) {}

	const T & x() const { return std::get<0>(*this); }
	void setX(const T & x) { std::get<0>(*this) = x; }

	const T & y() const { return std::get<1>(*this); }
	void setY(const T & y) { std::get<1>(*this) = y; }

	template <class U>
	PointT<U> to() const { return PointT<U>(x(), y()); }
};

using PointFT = PointT<float>;

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_POINT_H_ */
