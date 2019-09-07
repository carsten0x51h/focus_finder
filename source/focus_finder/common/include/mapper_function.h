/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_MAPPER_FUNCTION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_MAPPER_FUNCTION_H_

#include <string>

class MapperFunctionT {
public:
	virtual std::string getName() const = 0;
	virtual float f(float f) = 0;

	virtual void setSrcRange(float from, float to) = 0;
	virtual void setDestRange(float from, float to) = 0;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_MAPPER_FUNCTION_H_ */
