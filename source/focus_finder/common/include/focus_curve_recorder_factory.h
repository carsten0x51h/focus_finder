#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_FACTORY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_FACTORY_H_

#include <memory>

#include "focus_curve_recorder_type.h"
#include "enum_helper.h"

class FocusCurveRecorderT;
class FocusControllerT;

class FocusCurveRecorderFactoryT {
public:
  static std::shared_ptr<FocusCurveRecorderT> getInstance(const FocusCurveRecorderTypeT::TypeE & type, std::shared_ptr<FocusControllerT> focusAnalyzer);
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_FACTORY_H_ */
