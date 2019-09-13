#include <memory>

#include "include/focus_curve_recorder_factory.h"
#include "include/default_focus_curve_recorder.h"

std::shared_ptr<FocusCurveRecorderT> FocusCurveRecorderFactoryT::getInstance(
		const FocusCurveRecorderTypeT::TypeE & type) {

	switch (type) {
	case FocusCurveRecorderTypeT::DEFAULT:
		return std::make_shared<DefaultFocusCurveRecorderT>();

	default:
		return nullptr;
	}
}
