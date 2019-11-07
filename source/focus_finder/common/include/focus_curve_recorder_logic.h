#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_LOGIC_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_LOGIC_H_

#include <memory>

#include "focus_finder_logic.h"
#include "focus_curve.h"
#include "fitting_curve_type.h"
#include "focus_curve_recorder.h"
#include "focus_curve_record_set.h"
#include "focus_curve_recorder_type.h"


class FocusCurveRecorderLogicT {
 private:
  FocusFinderLogicT & mFfl;

  std::shared_ptr<FocusCurveRecorderT> mFocusCurveRecorder;

  FocusMeasureTypeT::TypeE mFocusMeasureType;
  FittingCurveTypeT::TypeE mFocusCurveType;
    
 public:
  FocusCurveRecorderLogicT(FocusFinderLogicT & ffl);


  std::shared_ptr<FocusCurveRecorderT> getFocusCurveRecorder();
  void resetFocusCurveRecorder(FocusCurveRecorderTypeT::TypeE focusCurveRecorderType);

  // FocusMeasureTypeT::TypeE getFocusMeasureType() const;
  // void setFocusMeasureType(FocusMeasureTypeT::TypeE focusMeasureType);

  FittingCurveTypeT::TypeE getFocusCurveType() const;
  void setFocusCurveType(FittingCurveTypeT::TypeE focusCurveType);
  
  bool checkDevices() const;

};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_LOGIC_H_*/
