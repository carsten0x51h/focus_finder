#include "include/curve_parm.h"

#include <fstream>
#include <string>

CurveParmT::CurveParmT() :
  mName(""), mValue(0.0f) {
}

CurveParmT::CurveParmT(const std::string & name, float value = 0.0f) :
		mName(name), mValue(value) {
}

const std::string & CurveParmT::getName() const {
	return mName;
}

float CurveParmT::getValue() const {
	return mValue;
}
void CurveParmT::setValue(float value) {
	mValue = value;
}

std::ostream &
CurveParmT::print(std::ostream & os) const {
	os << mName << " = " << mValue;
	return os;
}

std::ostream & operator<<(std::ostream & os, const CurveParmT & curveParm) {
	return curveParm.print(os);
}
