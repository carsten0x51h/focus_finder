#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_PARMS_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_PARMS_H_

#include <vector>

#include "exception.h"
#include "curve_parm.h"

/**
 * Usage:
 *
 * myCurveParms.get(IDX_B).getValue();
 * myCurveParms.get(IDX_B).getName();
 */
class CurveParmsT {
private:
	std::vector<CurveParmT> mCurveParms;

public:
  CurveParmsT();
  CurveParmsT(size_t size);
	size_t size() const;
	const CurveParmT & get(size_t idx) const;
	const CurveParmT & get(const std::string & name) const;
	CurveParmT & operator[](size_t idx);

	std::ostream & print(std::ostream & os) const;
	friend std::ostream & operator<<(std::ostream & os, const CurveParmsT & curveParms);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_PARMS_H_*/
