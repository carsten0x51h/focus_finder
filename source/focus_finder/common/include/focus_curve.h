#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_

#include <fstream>

class FocusCurveT {
private:

public:
  //	FocusCurveT();
  //	~FocusCurveT();

	std::ostream & print(std::ostream & os) const;
	friend std::ostream & operator<<(std::ostream & os, const FocusCurveT & focusCurve);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_*/




