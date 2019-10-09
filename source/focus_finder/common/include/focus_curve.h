#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_

#include <memory>
#include <fstream>
#include <vector>

#include "focus_curve_record.h"
#include "image.h"
#include "point.h"

class FocusCurveT : public std::vector< std::shared_ptr<FocusCurveRecordT> > {
private:
  //	std::vector< std::shared_ptr<FocusCurveRecordT> > mFocusCheckpoints;

public:
  //	FocusCurveT();
  //	~FocusCurveT();

	std::ostream & print(std::ostream & os) const;
	friend std::ostream & operator<<(std::ostream & os, const FocusCurveT & focusCurve);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_*/




