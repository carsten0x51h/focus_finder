#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_POINT_WITH_RESIDUAL_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_POINT_WITH_RESIDUAL_H_

#include <ostream>

#include "point.h"

class PointWithResidualT {
public:
	PointWithResidualT(const PointFT & point, float residual);

	PointFT point;
	float residual;

    friend std::ostream & operator<<(std::ostream & os, const PointWithResidualT & pointWithResidual);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_POINT_WITH_RESIDUAL_H_*/

