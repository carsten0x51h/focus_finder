#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_PARM_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_PARM_H_

#include <fstream>
#include <string>

#include "exception.h"

DEF_Exception(CurveParm);

class CurveParmT {
private:
	std::string mName;
	float mValue;

public:
  CurveParmT();
  CurveParmT(const std::string & name, float value);

	const std::string & getName() const;
	float getValue() const;
	void setValue(float value);

	std::ostream & print(std::ostream & os) const;
	friend std::ostream & operator<<(std::ostream & os, const CurveParmT & curveParm);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_PARM_H_*/
