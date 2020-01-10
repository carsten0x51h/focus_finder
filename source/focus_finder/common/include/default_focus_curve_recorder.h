#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEFAULT_FOCUS_CURVE_RECORDER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEFAULT_FOCUS_CURVE_RECORDER_H_

//#include <thread>
//#include <atomic>
//#include <condition_variable>
//#include <boost/signals2.hpp>

#include "focus_curve_recorder.h"
//#include "focus_curve_record_set.h"
#include "focus_measure_type.h"
#include "curve_half.h"
//#include "focus_controller.h"

// TODO / IDEA: Maybe HfdT and FwhmT should both implement a generic inteface "FocusMeasureT" which just hast "float getValue()".

class FocusCurveRecordSetT;
class FocusControllerT;

class DefaultFocusCurveRecorderT: public FocusCurveRecorderT {
private:
  // Prevent copy
  DefaultFocusCurveRecorderT(const DefaultFocusCurveRecorderT &);
  DefaultFocusCurveRecorderT & operator=(const DefaultFocusCurveRecorderT &);
  
  FocusMeasureTypeT::TypeE getLimitFocusMeasureType() const;
  FocusMeasureTypeT::TypeE getCurveFocusMeasureType() const;
  float getFocusMeasureLimit() const;
  int getStepSize() const;
  
  //void cleanup();
  void checkCancelled() const;
  CurveHalfT::TypeE locateStartingPosition();
  std::shared_ptr<FocusCurveRecordSetT> recordFocusCurveRecordSet(CurveHalfT::TypeE curveHalf);

  std::atomic<bool> mCancelled; // TODO: Still required?
  std::atomic<bool> mIsRunning;

  std::shared_ptr<FocusCurveRecordSetContainerT> mFocusCurveRecordSets;

public:
  DefaultFocusCurveRecorderT(std::shared_ptr<FocusControllerT> focusController);
  
  // Implement focus curve recorder interface
  std::string getName() const override;
  
  bool isRunning() const override;
  void run() override;
  void cancel() override;
  void reset() override;

  std::shared_ptr<const FocusCurveRecordSetContainerT> getFocusCurveRecordSets() const override;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEFAULT_FOCUS_CURVE_RECORDER_H_*/
