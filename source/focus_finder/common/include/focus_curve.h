#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_

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

  float mLowerFocusPos; // TODO: Required?
  float mUpperFocusPos; // TODO: Required?
  float mFocusMeasureLimit;

  std::time_t mDateTime;
  
  // TODO: Should CurveFitSummaryT be part of this class?
  //CurveFitSummaryT mCurveFitSummary;          // < Contains details about the curve fit (success, outlier boundary, outliers, matched data points, ...)

  std::shared_ptr<CurveFunctionT> mFocusCurveFunction;

public:
  FocusCurveT(std::shared_ptr<const FocusCurveRecordSetT> focusCurveRecordSet, const CurveFitParmsT & curveFitParms);
  ~FocusCurveT();
  
  float getLowerFocusPos() const;
  float getUpperFocusPos() const;

  float getBestAbsFocusPos() const;
  float getRelativeFocusPosBoundary() const;
  
  float calcFocusMeasureByFocusPosition(float focusPosition) const;

  static std::vector<float> calcFocusPositionByFocusMeasure(std::shared_ptr<const CurveFunctionT> curveFunction, float focusMeasure);
  std::vector<float> calcFocusPositionByFocusMeasure(float focusMeasure) const;

  std::time_t getDateTime() const;
  FocusMeasureTypeT::TypeE getFocusMeasureType() const;
  FocusCurveTypeT::TypeE getFocusCurveType() const;
  const CurveFitSummaryT & getCurveFitSummary() const;
  const CurveParmsT & getCurveParms() const;
  
  std::ostream & print(std::ostream & os) const;
  
  friend std::ostream & operator<<(std::ostream & os, const FocusCurveT & focusCurve);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_H_*/




