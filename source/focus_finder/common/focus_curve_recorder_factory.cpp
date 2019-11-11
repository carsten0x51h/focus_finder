#include <memory>

#include "include/focus_curve_recorder_factory.h"
#include "include/default_focus_curve_recorder.h"

class FocusControllerT;

std::shared_ptr<FocusCurveRecorderT> FocusCurveRecorderFactoryT::getInstance(
									     const FocusCurveRecorderTypeT::TypeE & type, std::shared_ptr<FocusControllerT> focusAnalyzer) {

	switch (type) {
	case FocusCurveRecorderTypeT::DEFAULT:
		return std::make_shared<DefaultFocusCurveRecorderT>(focusAnalyzer);

	default:
		return nullptr;
	}
}
