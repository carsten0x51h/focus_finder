#include "include/focus_curve_recorder_logic.h"

#include "include/focus_finder_logic.h"
#include "include/point.h"
#include "include/point_with_residual.h"
#include "include/focus_curve_recorder_type.h"
#include "include/focus_curve_recorder_factory.h"
#include "include/focus_curve_recorder.h"
#include "include/profile_manager.h"
#include "include/focus_curve.h"
#include "include/focus_controller.h"

class FocusCurveRecordSetT;

FocusCurveRecorderLogicT::FocusCurveRecorderLogicT(FocusFinderLogicT & ffl) : mFfl(ffl), mFocusCurveRecorder(nullptr), mFocusCurveType(FittingCurveTypeT::HYPERBOLIC) {
}

std::shared_ptr<FocusCurveRecorderT> FocusCurveRecorderLogicT::getFocusCurveRecorder() {
  return mFocusCurveRecorder;
}

void FocusCurveRecorderLogicT::resetFocusCurveRecorder(FocusCurveRecorderTypeT::TypeE focusCurveRecorderType) {

  auto focusController = std::make_shared<FocusControllerT>(mFfl.getCurrentCamera(), mFfl.getCurrentFocus(), mFfl.getCurrentFilter());
  auto lastFocusStarPosOpt = mFfl.getLastFocusStarPos();

  //if (!lastFocusStarPosOpt) {
  // TODO: Handle case where no focus star is set / available
  //}
  focusController->setLastFocusStarPos(lastFocusStarPosOpt.value());


  auto activeProfileOpt = mFfl.getProfileManager()->getActiveProfile();

  // TODO: Make sure that activeProfile is set...
  //if (!activeProfileOpt) {
  //}
  focusController->setFocusFinderProfile(activeProfileOpt.value());

  
  mFocusCurveRecorder = FocusCurveRecorderFactoryT::getInstance(
								focusCurveRecorderType,
								focusController);
}

FittingCurveTypeT::TypeE FocusCurveRecorderLogicT::getFocusCurveType() const {
  return mFocusCurveType;
}

void FocusCurveRecorderLogicT::setFocusCurveType(FittingCurveTypeT::TypeE focusCurveType) {
     mFocusCurveType = focusCurveType;
}
