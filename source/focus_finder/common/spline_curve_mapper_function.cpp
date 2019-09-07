#include "include/spline_curve_mapper_function.h"

#include "include/logging.h"
#include "include/mapper_function.h"


SplineCurveMapperFunctionT::SplineCurveMapperFunctionT() :
	mSrcFrom(0),
	mSrcTo(0),
	mDestFrom(0),
	mDestTo(0)
{

}

SplineCurveMapperFunctionT::~SplineCurveMapperFunctionT() {

}

std::string SplineCurveMapperFunctionT::getName() const {
	return "SplineCurveMapperFunction";
}

// TODO: Maybe move to parent class
void SplineCurveMapperFunctionT::setSrcRange(float from, float to) {
	// TODO: Check that from <= to
	LOG(debug) << "SplineCurveMapperFunctionT::setSrcRange... ["<< from << ", " << to << "]" << std::endl;

	mSrcFrom = from;
	mSrcTo = to;
}

// TODO: Maybe move to parent class
void SplineCurveMapperFunctionT::setDestRange(float from, float to) {
	// TODO: Check that from <= to
	LOG(debug) << "SplineCurveMapperFunctionT::setDestRange... ["<< from << ", " << to << "]" << std::endl;

	mDestFrom = from;
	mDestTo = to;
}

float SplineCurveMapperFunctionT::f(float f) {
	return f;
}

