/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c [at] lost-infinity.com>
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

//#include <thread> // TODO: Remove - only temporary
#include <chrono>
#include <utility>

#include "include/single_pass_focusing_strategy.h"
#include "include/logging.h"
#include "include/exception.h"
#include "include/timeout_exception.h"

#include "include/snr.h"
#include "include/fwhm.h"
#include "include/focus_curve_record_set.h"
#include "include/curve_function_factory.h"
#include "include/curve_function.h"
#include "include/focus_curve.h"

class FocusControllerT;

// TODO: Find a better name for this class.... we don't really lookup something...
SinglePassFocusingStrategyT::SinglePassFocusingStrategyT(std::shared_ptr<FocusControllerT> focusController)
        : FocusFinderT(std::move(focusController)),
          mCancelled(false),
          mIsRunning(false) {
    LOG(debug)
        << "SinglePassFocusingStrategyT::SinglePassFocusingStrategyT..." << std::endl;
}

std::string SinglePassFocusingStrategyT::getName() const {
    return "SinglePassFocusingStrategyT";
}

void SinglePassFocusingStrategyT::reset() {
    // TODO
    // TODO: What happens if called while running? -> Exception?
    LOG(debug)
        << "SinglePassFocusingStrategyT::reset..." << std::endl;
}

void SinglePassFocusingStrategyT::checkCancelled() const {
    if (mCancelled.load()) {
        throw FocusControllerCancelledExceptionT("Focus finder cancelled.");
    }
}

bool SinglePassFocusingStrategyT::isRunning() const {
    return mIsRunning.load();
}


/**
 * Process
 * -------
 *
 * // Determine curveHalf -> direction to move to reach best focus
 * measure()
 * move(factor * stepSize)
 * measure()
 *
 * while(...)
 *    move(stepSize)
 *    measure()
 *
 * move(rest)  // Move rest of the distance
 * measure()   // Final measurement
 */
void SinglePassFocusingStrategyT::run() {
    LOG(debug) << "SinglePassFocusingStrategyT::run..." << std::endl;

    mIsRunning = true;

    // TODO: Check that device manager is set

    // TODO: Check if focus controller is set


    auto focusFinderProfile = getFocusController()->getFocusFinderProfile();

    // TODO: Crosscheck if current profile has calibration-data... use hasCalibrationData()...

    auto foFiCalibration = focusFinderProfile.getFocusFinderCalibration();
    std::shared_ptr<FocusCurveT> focusCurve = foFiCalibration->getFocusCurve();
    FittingCurveTypeT::TypeE curveFittingType = FocusCurveTypeT::toFittingCurve(focusCurve->getFocusCurveType());
    const CurveParmsT &parms = focusCurve->getCurveParms();

    auto curveFocusMeasureType = focusFinderProfile.getCurveFocusMeasureType();
    float focusMeasureLimit = focusFinderProfile.getFocusMeasureLimit();
    int foFiStepSize = focusFinderProfile.getStepSize();
    std::shared_ptr<FocusCurveRecordSetT> records = std::make_shared<FocusCurveRecordSetT>(curveFocusMeasureType,
                                                                                           focusMeasureLimit);

    LOG(debug) << "SinglePassFocusingStrategyT::run... Using parameters: " << std::endl
               << "  > curveFittingType=" << FittingCurveTypeT::asStr(curveFittingType) << std::endl
               << "  > foFiStepSize=" << foFiStepSize << std::endl
               << "  > curveFocusMeasureType=" << FocusMeasureTypeT::asStr(curveFocusMeasureType)
               << "  > focusMeasureLimit=" << focusMeasureLimit << std::endl
               << "  > curve parameters:" << std::endl
               << parms
               << std::endl;

//    auto focusCurveFunction = CurveFunctionFactoryT::getInstance(curveFittingType, parms);

    try {
        using namespace std::chrono_literals;

        // TODO: Check that camera is there  --> use focus controller checkDeviceAvailable() function
        // TODO: Check that focus is there --> use focus controller checkDeviceAvailable() function


        // Notify that focus finder started
        notifyFocusFinderStarted();

        // TODO: Set the filter...


        // Read the current focus controller
        mInitialAbsPosition = getFocusController()->getFocus()->getCurrentPos();

        LOG(debug) << "SinglePassFocusingStrategyT::run... current focus position: " << mInitialAbsPosition
                   << std::endl;


        // Self orientation
        LOG(debug)
            << "SinglePassFocusingStrategyT::run - Self orientation..." << std::endl;

        SelfOrientationResultT selfOrientationResult = getFocusController()->performSelfOrientation(focusMeasureLimit);

        LOG(debug)
            << "SinglePassFocusingStrategyT::run - Self orientation result:"
            << selfOrientationResult
            << std::endl;


        CurveSectorT::TypeE curveHalf = selfOrientationResult.curveHalf;

        records->push_back(selfOrientationResult.record1);
        records->push_back(selfOrientationResult.record2);

        // Sort the measurements by abs. position
        // auto sortMeasuresByAbsPosLambda = [] (std::shared_ptr<const FocusCurveRecordT> c1, std::shared_ptr<const FocusCurveRecordT> c2) -> bool
        // 				      {
        // 					return c1->getCurrentAbsoluteFocusPos() < c2->getCurrentAbsoluteFocusPos();
        // 				      };

        // TODO: Is this required here?
        //std::sort(records->begin(), records->end(), sortMeasuresByAbsPosLambda);




        // DEBUG START
        LOG(debug) << "SinglePassFocusingStrategyT::Measured values by self orientation" << std::endl
                   << "SinglePassFocusingStrategyT::------------------------------------" << std::endl;

        for (const auto& record : *records) {
            float focusMeasure = record->getFocusMeasure(curveFocusMeasureType);

            LOG(debug) << "SinglePassFocusingStrategyT::Focus measure=" << focusMeasure << " ("
                       << FocusMeasureTypeT::asStr(curveFocusMeasureType) << ")"
                       << ", abs focus pos=" << record->getCurrentAbsoluteFocusPos()
                       << std::endl;
        }
        // DEBUG END


        // TODO: Implement strategy...


        // Notify that FoFi was finished...
        notifyFocusFinderFinished();


    } catch (FocusControllerCancelledExceptionT &exc) {
        focusFinderCleanup();

        // TODO: Improve error handling... -> ReportingT?
        LOG(warning)
            << "Focus finder was cancelled." << std::endl;

        // Notify that FoFi was cancelled...
        notifyFocusFinderCancelled();
    } catch (FocusExceptionT &exc) {
        // TODO: MAybe write one common exception handler for the FoFi here since code repeats!
        focusFinderCleanup();

        // TODO: Improve error handling... -> ReportingT?
        LOG(error)
            << "Hit focus exception: " << exc.what() << std::endl;

        // Notify that FoFi was cancelled...
        // TODO: Maybe introduce notifyFocusFinderFailed()...
        notifyFocusFinderCancelled();

    } catch (TimeoutExceptionT &exc) {
        focusFinderCleanup();

        // TODO: Improve error handling... -> ReportingT?
        LOG(error)
            << "Hit timeout, unable to find focus." << std::endl;

        // Notify that FoFi was cancelled...
        // TODO: Maybe introduce notifyFocusFinderFailed()...
        notifyFocusFinderCancelled();
    }

    mIsRunning = false;
}

void SinglePassFocusingStrategyT::rollbackFocus() {
    using namespace std::chrono_literals;

    LOG(debug)
        << "SinglePassFocusingStrategyT::rollbackFocus..." << std::endl;

    getFocusController()->moveFocusToBlocking(mInitialAbsPosition, 30000ms);
}

void SinglePassFocusingStrategyT::focusFinderCleanup() {
    LOG(debug)
        << "SinglePassFocusingStrategyT::focusFinderCleanup..." << std::endl;

    rollbackFocus();
}

void SinglePassFocusingStrategyT::cancel() {
    mCancelled = true;
    getFocusController()->cancel();
}

///**********************************************************************
// * Calculation of asymptotes / slopes
// **********************************************************************/
//float left_slope(float x, float a, float b, float c, float d) {
//  return -(b/a) * (x-c) + d;
//}
//float right_slope(float x, float a, float b, float c, float d) {
//  return (b/a) * (x-c) + d;
//}
