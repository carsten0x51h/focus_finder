#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_LOGIC_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_LOGIC_H_

#include <memory>

#include "focus_finder_logic.h"
#include "focus_curve.h"
#include "focus_curve_recorder.h"
#include "focus_curve_record_set.h"
#include "focus_curve_recorder_type.h"


class FocusCurveRecorderLogicT {
 private:
  FocusFinderLogicT & mFfl;

  std::shared_ptr<FocusCurveRecorderT> mFocusCurveRecorder;

 public:
  FocusCurveRecorderLogicT(FocusFinderLogicT & ffl);

  std::shared_ptr<FocusCurveRecorderT> getFocusCurveRecorder();
  void resetFocusCurveRecorder(FocusCurveRecorderTypeT::TypeE focusCurveRecorderType);

  std::optional<FocusFinderProfileT> getActiveProfile();
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_LOGIC_H_*/
