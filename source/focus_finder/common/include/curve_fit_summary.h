/*
 * camera.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FIT_SUMMARY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FIT_SUMMARY_H_

#include <ostream>
#include <vector>

#include "point.h"
#include "point_with_residual.h"

class CurveFitSummaryT {
public:
	bool success;								// < Indicates if curve parameters were found under given constraints
	size_t numIterationsRequiredTotal = 0;		// < Number of required iterations in total
    float outlierBoundary;						// < Contains outlier boundary in case outlier detection was enabled, otherwise 0
    std::vector<PointWithResidualT> outliers;	// < Contains outliers in case outlier detection was enabled
    std::vector<PointFT> matchedDataPoints;		// < Data points which were used to calculate the curve parameters
    std::vector<PointFT> curveDataPoints;		// < Corresponding data points to matchedDataPoints on curve

    friend std::ostream & operator<<(std::ostream & os, const CurveFitSummaryT & summary);
};


#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FIT_SUMMARY_H_*/
