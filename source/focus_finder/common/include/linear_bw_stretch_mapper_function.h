/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LINEAR_BW_STRETCH_MAPPER_FUNCTION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LINEAR_BW_STRETCH_MAPPER_FUNCTION_H_

#include <string>

#include "mapper_function.h"

class LinearBWStretchMapperFunctionT : public MapperFunctionT {
public:
	LinearBWStretchMapperFunctionT();
	virtual ~LinearBWStretchMapperFunctionT();

	std::string getName() const;

	// TODO: Maybe move to parent class
	void setSrcRange(float from, float to);

	// TODO: Maybe move to parent class
	void setDestRange(float from, float to);

	float f(float f);

	void setBlackPoint(float blackPoint);
	void setWhitePoint(float whitePoint);

private:
	float mBlackPoint;
	float mWhitePoint;

	// TODO: Maybe move to parent class
	float mSrcFrom;
	float mSrcTo;
	float mDestFrom;
	float mDestTo;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LINEAR_BW_STRETCH_MAPPER_FUNCTION_H_*/
