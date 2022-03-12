/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c [at] lost-infinity.com>
 *
 *  More info on https://www.lost-infinity.com
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FIT_SUMMARY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FIT_SUMMARY_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FIT_SUMMARY_H_

#include <ostream>
#include <vector>

#include "point.h"
#include "point_with_residual.h"

struct CurveFitSummaryT {
    CurveFitSummaryT();

    bool success;                                // < Indicates if curve parameters were found under given constraints
    size_t numIterationsRequiredTotal = 0;       // < Number of required iterations in total
    float outlierBoundary;                       // < Contains outlier boundary in case outlier detection was enabled, otherwise 0
    std::vector<PointWithResidualT> outliers;    // < Contains outliers in case outlier detection was enabled
    std::vector<PointFT> matchedDataPoints;      // < Data points which were used to calculate the curve parameters
    std::vector<PointFT> curveDataPoints;        // < Corresponding data points to matchedDataPoints on curve

    friend std::ostream &operator<<(std::ostream &os, const CurveFitSummaryT &summary);
};


#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FIT_SUMMARY_H_*/
