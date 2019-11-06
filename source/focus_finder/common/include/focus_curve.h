#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_

#include <fstream>
#include <memory>
#include <ctime>

#include "fitting_curve_type.h"
#include "focus_measure_type.h"
#include "curve_parms.h"
#include "curve_function.h"
#include "curve_fit_summary.h"

// TODO: need further includes...

class FocusCurveRecordSetT;

class FocusCurveT {
private:
  FittingCurveTypeT::TypeE mFocusCurveType;   // < Curve type used to match the focus curve - e.g. HYPERBOLIC
  CurveParmsT mCurveParms;                    // < The resulting curve parameters from the match
  CurveFitSummaryT mCurveFitSummary;

  FocusMeasureTypeT::TypeE mFocusMeasureType; // < Focus measure type used to record the data points - e.g. HFD

  float mLowerFocusPos; // TODO: Required?
  float mUpperFocusPos; // TODO: Required?
  float mFocusMeasureLimit;

  std::time_t mDateTime;
  
  // TODO: Should CurveFitSummaryT be part of this class?
  //CurveFitSummaryT mCurveFitSummary;          // < Contains details about the curve fit (success, outlier boundary, outliers, matched data points, ...)

  std::shared_ptr<CurveFunctionT> mFocusCurveFunction;

public:
  static const float EPS_REL;
  static const float EPS_ABS;
  static const size_t MAX_NUM_ITER;
  static const float OUTLIER_BOUNDARY_FACTOR;
  static const float MAX_ACCEPTED_OUTLIERS_PERC;

  // FittingCurveTypeT::TypeE curveType, const CurveParmsT & curveParms, FocusMeasureTypeT::TypeE focusMeasureType, int lowerFocusPos, int upperFocusPos, float focusMeasureLimit
  FocusCurveT(std::shared_ptr<const FocusCurveRecordSetT> focusCurveRecordSet, FittingCurveTypeT::TypeE curveType);

  ~FocusCurveT();
  
  float getLowerFocusPos() const;
  float getUpperFocusPos() const;
  
  float calcFocusMeasureByFocusPosition(float focusPosition) const;
  float calcFocusPositionByFocusMeasure(float focusMeasure) const;

  std::time_t getDateTime() const;
  FocusMeasureTypeT::TypeE getFocusMeasureType() const;
  FittingCurveTypeT::TypeE getFocusCurveType() const;
  const CurveFitSummaryT & getCurveFitSummary() const;

  std::ostream & print(std::ostream & os) const;
  
  friend std::ostream & operator<<(std::ostream & os, const FocusCurveT & focusCurve);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_*/




