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
#include <gsl/gsl_errno.h> // Definition of GSL_SUCCESS, GSL_FAILURE, ...

#include "include/lm_curve_matcher_summary.h"


LmCurveMatcherSummaryT::LmCurveMatcherSummaryT() : statusCode(GSL_FAILURE), numIterationsRequired(0) {}

std::ostream &operator<<(std::ostream &os,
                         const LmCurveMatcherSummaryT &lmCurveMatcherSummary) {
    static const char *prefix = "   > ";

    os << "** LmCurveMatcherSummary **" << std::endl;
    os << prefix << "success: " << lmCurveMatcherSummary.isSuccessful() << std::endl
       << prefix << "numIterationsRequired: " << lmCurveMatcherSummary.numIterationsRequired << std::endl
       << prefix << "statusCode: " << lmCurveMatcherSummary.statusCode << std::endl
       << prefix << "statusMsg: " << lmCurveMatcherSummary.statusMsg << std::endl;

    return os;
}

bool LmCurveMatcherSummaryT::isSuccessful() const {
    return ( GSL_SUCCESS == statusCode );
}
