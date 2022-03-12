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

#include <ostream>
#include <iomanip>

#include "include/point_with_residual.h"
#include "include/curve_fit_summary.h"

CurveFitSummaryT::CurveFitSummaryT() : success(false), outlierBoundary(0.0F) {}


std::ostream &operator<<(std::ostream &os, const CurveFitSummaryT &summary) {
    static const char *prefix = "   > ";

    os << "** CurveFitSummary **" << std::fixed << std::setprecision(20) << std::endl;

    // success
    os << prefix << "Curve fitting status" << std::endl;
    os << prefix << (summary.success ? "SUCCESS" : "FAILURE") << std::endl;

    // matchedDataPoints
    os << prefix << "Matched data points [#dp=" << summary.matchedDataPoints.size() << "]" << std::endl;

    for (const auto &p : summary.matchedDataPoints) {
        os << prefix << p << std::endl;
    }
    os << std::endl;

    // curveDataPoints
    os << prefix << "Curve data points [#dp=" << summary.curveDataPoints.size() << "]" << std::endl;

    for (const auto &p : summary.curveDataPoints) {
        os << prefix << p << std::endl;
    }
    os << std::endl;


    // outliers
    bool outlierDetectionEnabled = (summary.outlierBoundary >= 0);

    if (outlierDetectionEnabled) {
        os << prefix << "Outliers [#outliers=" << summary.outliers.size() << ", boundary=" << summary.outlierBoundary << "]"
           << std::endl;

        for (const auto &pr : summary.outliers) {
            os << prefix << pr << std::endl;
        }
        // TODO: For some reason that does not compile...
        // std::copy(summary.outliers.begin(),
        // 		  summary.outliers.end(),
        // 		  std::ostream_iterator<PointWithResidualT>(os));

    } else {
        os << prefix << "Outliers [detection disabled]" << std::endl;
    }
    os << std::endl;

    os << prefix << "Number of iterations total=" << summary.numIterationsRequiredTotal << std::endl;

    return os;
}
