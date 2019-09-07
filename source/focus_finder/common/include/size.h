/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SIZE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SIZE_H_

#include <tuple>

#include "tuple_printer.h"

/**
 * SizeT structure (W x H).
 */
template <class T>
class SizeT : public std::tuple<T,T> {
public:
	SizeT() : std::tuple<T,T>() {}
	SizeT(const T & w, const T & h) : std::tuple<T,T>(w,h) {}

	const T & width() const { return std::get<0>(*this); }
	void setWidth(const T & w) { std::get<0>(*this) = w; }

	const T & height() const { return std::get<1>(*this); }
	void setHeight(const T & h) { std::get<1>(*this) = h; }

	template <class U>
	SizeT<U> to() const { return SizeT<U>(width(), height()); }

	SizeT<T> operator* (T factor) { return SizeT<T>(width() * factor, height() * factor); }
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SIZE_H_ */

