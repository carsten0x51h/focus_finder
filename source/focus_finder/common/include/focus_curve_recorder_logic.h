#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_LOGIC_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_LOGIC_H_

#include <memory>

#include "focus_finder_logic.h"
#include "focus_curve_recorder.h"
#include "focus_curve_record_set.h"
#include "focus_curve_recorder_type.h"


class FocusCurveRecorderLogicT {
 private:
  FocusFinderLogicT & mFfl;

  std::shared_ptr<FocusCurveRecorderT> mFocusCurveRecorder;


  static const float EPS_REL;
  static const float EPS_ABS;
  static const size_t MAX_NUM_ITER;
  static const float OUTLIER_BOUNDARY_FACTOR;
  static const float MAX_ACCEPTED_OUTLIERS_PERC;
  
 public:
  FocusCurveRecorderLogicT(FocusFinderLogicT & ffl);


  std::shared_ptr<FocusCurveRecorderT> getFocusCurveRecorder();
  void resetFocusCurveRecorder(FocusCurveRecorderTypeT::TypeE focusCurveRecorderType);
  
  
  bool checkDevices() const;
  
  static void fitFocusCurve(std::shared_ptr<const FocusCurveRecordSetT> focusCurveRecordSet);

};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_LOGIC_H_*/
