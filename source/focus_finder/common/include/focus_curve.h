/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c.schmitt51h@gmail.com>
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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_

#include <fstream>
#include <memory>
#include <ctime>

#include "focus_curve_type.h"
#include "focus_measure_type.h"
#include "curve_parms.h"
#include "curve_fit_parms.h"
#include "curve_function.h"
#include "curve_fit_summary.h"

// TODO: need further includes...

class FocusCurveRecordSetT;

class FocusCurveT {
private:
    FocusCurveTypeT::TypeE mFocusCurveType;   // < Curve type used to match the focus curve - e.g. HYPERBOLIC
    CurveParmsT mCurveParms;                    // < The resulting curve parameters from the match
    CurveFitSummaryT mCurveFitSummary;

    FocusMeasureTypeT::TypeE mFocusMeasureType; // < Focus measure type used to record the data points - e.g. HFD

    float mLowerFocusPos{}; // TODO: Required?
    float mUpperFocusPos{}; // TODO: Required?
    float mFocusMeasureLimit;

    std::time_t mDateTime;

    // TODO: Should CurveFitSummaryT be part of this class?
    //CurveFitSummaryT mCurveFitSummary;          // < Contains details about the curve fit (success, outlier boundary, outliers, matched data points, ...)

    std::shared_ptr<CurveFunctionT> mFocusCurveFunction;

public:
    FocusCurveT(const std::shared_ptr<const FocusCurveRecordSetT>& focusCurveRecordSet, const CurveFitParmsT &curveFitParms);

    ~FocusCurveT();

    [[nodiscard]] float getLowerFocusPos() const;

    [[nodiscard]] float getUpperFocusPos() const;

    [[nodiscard]] float getBestAbsFocusPos() const;

    [[nodiscard]] float getRelativeFocusPosBoundary() const;

    [[nodiscard]] float calcFocusMeasureByFocusPosition(float focusPosition) const;

    static std::vector<float>
    calcFocusPositionByFocusMeasure(const std::shared_ptr<const CurveFunctionT>& curveFunction, float focusMeasure);

    [[nodiscard]] std::vector<float> calcFocusPositionByFocusMeasure(float focusMeasure) const;

    [[nodiscard]] std::time_t getDateTime() const;

    [[nodiscard]] FocusMeasureTypeT::TypeE getFocusMeasureType() const;

    [[nodiscard]] FocusCurveTypeT::TypeE getFocusCurveType() const;

    [[nodiscard]] const CurveFitSummaryT &getCurveFitSummary() const;

    [[nodiscard]] const CurveParmsT &getCurveParms() const;

    std::ostream &print(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const FocusCurveT &focusCurve);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_*/




