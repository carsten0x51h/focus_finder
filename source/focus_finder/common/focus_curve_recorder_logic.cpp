#include "include/focus_curve_recorder_logic.h"

#include "include/focus_finder_logic.h"
#include "include/point.h"
#include "include/point_with_residual.h"
#include "include/focus_curve_recorder_type.h"
#include "include/focus_curve_recorder_factory.h"
#include "include/focus_curve_recorder.h"
#include "include/profile_manager.h"
#include "include/focus_curve.h"

class FocusCurveRecordSetT;

FocusCurveRecorderLogicT::FocusCurveRecorderLogicT(FocusFinderLogicT & ffl) : mFfl(ffl), mFocusCurveRecorder(nullptr), mFocusMeasureType(FocusMeasureTypeT::HFD), mFocusCurveType(FittingCurveTypeT::HYPERBOLIC) {
}

std::shared_ptr<FocusCurveRecorderT> FocusCurveRecorderLogicT::getFocusCurveRecorder() {
  return mFocusCurveRecorder;
}

void FocusCurveRecorderLogicT::resetFocusCurveRecorder(FocusCurveRecorderTypeT::TypeE focusCurveRecorderType) {

    auto focusCurveRecorder = FocusCurveRecorderFactoryT::getInstance(
							focusCurveRecorderType);

    // Set devices
    focusCurveRecorder->setCamera(mFfl.getCurrentCamera());
    focusCurveRecorder->setFocus(mFfl.getCurrentFocus());
    focusCurveRecorder->setFilter(mFfl.getCurrentFilter());

    focusCurveRecorder->setFocusMeasureType(mFocusMeasureType);
    
      
    auto lastFocusStarPosOpt = mFfl.getLastFocusStarPos();

    //if (!lastFocusStarPosOpt) {
    // TODO: Handle case where no focus star is set / available
    //}
		
    focusCurveRecorder->setLastFocusStarPos(lastFocusStarPosOpt.value());

    auto activeProfileOpt = mFfl.getProfileManager()->getActiveProfile();
    
    //if (!activeProfileOpt) {
    // TODO: Make sure that activeProfile is set...
    //}
    focusCurveRecorder->setFocusFinderProfile(activeProfileOpt.value());

    
    //auto activeProfileOpt = mFfl.getProfileManager()->getActiveProfile();

    // TODO: Make sure that activeProfile is set...
    //		if (!activeProfileOpt) {
    //		}
    // TODO: Implement
    //focusCurveRecorder->setFocusFinderProfile(activeProfileOpt.value());

    mFocusCurveRecorder = focusCurveRecorder;
}


bool FocusCurveRecorderLogicT::checkDevices() const {
  auto activeCamera = mFfl.getCurrentCamera();
  auto activeFocuser = mFfl.getCurrentCamera();
  
  if (!activeCamera) {
    LOG(warning)
      << "No camera set!" << std::endl;
    return false;
  }
  
  if (!activeFocuser) {
    LOG(warning)
      << "No focuser set!" << std::endl;
    return false;
  }
  return true;
}

FocusMeasureTypeT::TypeE FocusCurveRecorderLogicT::getFocusMeasureType() const {
  return mFocusMeasureType;
}

void FocusCurveRecorderLogicT::setFocusMeasureType(FocusMeasureTypeT::TypeE focusMeasureType) {
  mFocusMeasureType = focusMeasureType;
}

FittingCurveTypeT::TypeE FocusCurveRecorderLogicT::getFocusCurveType() const {
  return mFocusCurveType;
}

void FocusCurveRecorderLogicT::setFocusCurveType(FittingCurveTypeT::TypeE focusCurveType) {
     mFocusCurveType = focusCurveType;
}
