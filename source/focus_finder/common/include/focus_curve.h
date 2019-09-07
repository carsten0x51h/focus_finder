#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_

#include <fstream>
#include <vector>

#include "focus_finder_record.h"
#include "image.h"
#include "point.h"

class FocusCurveT {
private:
	std::vector<FocusFinderRecordT> mFocusCheckpoints;

public:
	FocusCurveT();
	~FocusCurveT();


	std::ostream & print(std::ostream & os) const;
	friend std::ostream & operator<<(std::ostream & os, const FocusCurveT & focusCurve);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_*/




