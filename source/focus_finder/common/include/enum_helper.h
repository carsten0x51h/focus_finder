/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 *
 *
 * TODO: Rename asType() to valueOf()!
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_ENUM_HELPER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_ENUM_HELPER_H_

#include <strings.h>

#define MAC_AS_TYPE(Type, E, Count)			\
  static inline Type##E as##Type(const char * inX) {	\
    for (size_t i = 0; i < Count; ++i) {		\
      Type##E type = static_cast<Type##E>(i);		\
      if (! strcasecmp(inX, asStr(type))) {		\
	return type;					\
      }							\
    }							\
    return Count;					\
  }

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_ENUM_HELPER_H_ */
