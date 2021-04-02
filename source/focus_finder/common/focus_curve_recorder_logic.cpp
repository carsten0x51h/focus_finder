/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c.schmitt51h@gmail.com>
 *
 *  More info on https://www.lost-infinity.com
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

#include "include/focus_curve_recorder_logic.h"

#include "include/focus_finder_logic.h"
#include "include/focus_curve_recorder_type.h"
#include "include/focus_curve_recorder_factory.h"
#include "include/focus_curve_recorder.h"
#include "include/profile_manager.h"
#include "include/focus_controller.h"

class FocusCurveRecordSetT;

FocusCurveRecorderLogicT::FocusCurveRecorderLogicT(FocusFinderLogicT &ffl) : mFfl(ffl), mFocusCurveRecorder(nullptr) {
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

void FocusCurveRecorderLogicT::resetFocusCurveRecorder(FocusCurveRecorderTypeT::TypeE focusCurveRecorderType,
                                                       const FocusFinderProfileT &focusFinderProfile) {

    auto focusController = std::make_shared<FocusControllerT>(mFfl.getCurrentCamera(), mFfl.getCurrentFocus(),
                                                              mFfl.getCurrentFilter());
    auto lastFocusStarPosOpt = mFfl.getLastFocusStarPos();

    //if (!lastFocusStarPosOpt) {
    // TODO: Handle case where no focus star is set / available
    //}
    focusController->setLastFocusStarPos(lastFocusStarPosOpt.value());


    //auto activeProfileOpt = mFfl.getProfileManager()->getActiveProfile();

    // TODO: Make sure that activeProfile is set...
    //if (!activeProfileOpt) {
    //}
    //focusController->setFocusFinderProfile(activeProfileOpt.value());

    focusController->setFocusFinderProfile(focusFinderProfile);

    mFocusCurveRecorder = FocusCurveRecorderFactoryT::getInstance(
            focusCurveRecorderType,
            focusController);
}
