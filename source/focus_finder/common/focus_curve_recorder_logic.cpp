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

FocusCurveRecorderLogicT::FocusCurveRecorderLogicT(FocusFinderLogicT & ffl) : mFfl(ffl), mFocusCurveRecorder(nullptr) {
}

std::shared_ptr<FocusCurveRecorderT> FocusCurveRecorderLogicT::getFocusCurveRecorder() {
  return mFocusCurveRecorder;
}

std::optional<FocusFinderProfileT> FocusCurveRecorderLogicT::getActiveProfile() {
  return mFfl.getProfileManager()->getActiveProfile();
}

std::shared_ptr<ProfileManagerT> FocusCurveRecorderLogicT::getProfileManager() const {
  return mFfl.getProfileManager();
}

  // TODO: Make sure that activeProfile is set...
  //if (!activeProfileOpt) {
  //}

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
