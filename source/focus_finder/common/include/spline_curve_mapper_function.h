/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SPLINE_CURVE_MAPPER_FUNCTION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SPLINE_CURVE_MAPPER_FUNCTION_H_

#include <string>

#include "mapper_function.h"

class SplineCurveMapperFunctionT : public MapperFunctionT {
public:
	SplineCurveMapperFunctionT();
	virtual ~SplineCurveMapperFunctionT();

	std::string getName() const;

	// TODO: Maybe move to parent class
	void setSrcRange(float from, float to);

	// TODO: Maybe move to parent class
	void setDestRange(float from, float to);

	float f(float f);

private:

	// TODO: Maybe move to parent class
	float mSrcFrom;
	float mSrcTo;
	float mDestFrom;
	float mDestTo;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SPLINE_CURVE_MAPPER_FUNCTION_H_*/
