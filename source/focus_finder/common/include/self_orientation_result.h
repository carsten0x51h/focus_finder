#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SELF_ORIENTATION_RESULT_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SELF_ORIENTATION_RESULT_H_

#include <memory>

#include "focus_direction.h"
#include "curve_half.h"

class FocusCurveRecordT;

struct SelfOrientationResultT {
  FocusDirectionT::TypeE focusDirectionToLimit;
  CurveHalfT::TypeE curveHalf;
  std::shared_ptr<FocusCurveRecordT> record1;
  std::shared_ptr<FocusCurveRecordT> record2;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SELF_ORIENTATION_RESULT_H_*/
