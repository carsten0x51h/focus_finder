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

//#include <thread> // TODO: Remove - only temporary
#include <chrono>
#include <utility>

#include "include/averaged_focus_curve_focusing_strategy.h"
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
AveragedFocsCurveFocusingStrategyT::AveragedFocsCurveFocusingStrategyT(std::shared_ptr<FocusControllerT> focusController)
        : FocusFinderT(std::move(focusController)),
          mCancelled(false),
          mIsRunning(false) {
    LOG(debug)
        << "AveragedFocsCurveFocusingStrategyT::AveragedFocsCurveFocusingStrategyT..." << std::endl;
}

std::string AveragedFocsCurveFocusingStrategyT::getName() const {
    return "AveragedFocsCurveFocusingStrategyT";
}

void AveragedFocsCurveFocusingStrategyT::reset() {
    // TODO
    // TODO: What happens if called while running? -> Exception?
    LOG(debug)
        << "AveragedFocsCurveFocusingStrategyT::reset..." << std::endl;
}

void AveragedFocsCurveFocusingStrategyT::checkCancelled() const {
    if (mCancelled.load()) {
        throw FocusControllerCancelledExceptionT("Focus finder cancelled.");
    }
}

bool AveragedFocsCurveFocusingStrategyT::isRunning() const {
    return mIsRunning.load();
}




// float AveragedFocsCurveFocusingStrategyT::estimateRelPos(std::shared_ptr<CurveFunctionT> focusCurveFunction, float focusMeasure, CurveSectorT::TypeE curveHalf) {

//   std::vector<float> estimatedFocusPositions = FocusCurveT::calcFocusPositionByFocusMeasure(focusCurveFunction, focusMeasure);

//   //std::vector<float> possibleFocusPositions2 = focusCurveFunction->f_inv(focusMeasure2);
//   //     std::vector<float> possibleFocusPositions = focusCurveFunction->f_inv(focusMeasure1);
//   //     if (curveHalf == ...) {
//   // 	return ...;
//   //     } else if () {
//   // 	return ...;
//   //     }
//   // }
//   //return 123;
// }


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
void AveragedFocsCurveFocusingStrategyT::run() {
    LOG(debug) << "AveragedFocsCurveFocusingStrategyT::run..." << std::endl;

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

    auto focusCurveFunction = CurveFunctionFactoryT::getInstance(curveFittingType, parms);

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


        CurveSectorT::TypeE curveHalf = selfOrientationResult.curveHalf; // TODO: Better name "CurveSectorT"?

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



        // TODO: This is problematic... e.g. if the seeing is better or worse (compared to when the curve was recorded)... then the curve shifts up/down.... and then the lookup of the measureValue points to a different relative position ... The only thing that would match would be the slope....
        //  -> Maybe it is possible to go to the other "curve half" and measure again
        //  -> IDEA: do a curve match with each new measure point for H(x,c,d) where c is the shift
        //           in x direction and d is the shift into y direction. c is the "perfect" focus pos.
        //  -> This way it is not even required to calculate f_inv / and or estimate the corresponding x position!

        // TODO: IMPLEMENT
        // TODO: IMPLEMENT
        // TODO: IMPLEMENT
        // TODO: IMPLEMENT
        // --> The actual question is: What will be used by the actual FocusFinder? -> probably just a subset of the CurveParmsT and some boundary values... so not the FocusCurveT object itself! From this a new CurveFunctionT object will be constructed.
        // -> So what is new, now... in the focus finder itself, now another curve fitting will take place. Again, we pass the curveShape/Type, the data points, some settings (error etc.) and - what is new - a list of fixed parameters
        // -> So that means, that we this time will create a new FocusCurve object with each new data point we measure! That means, the following call
        //
        //auto focusCurve = std::make_shared<FocusCurveT>(focusCurveRecordSet, mFocusCurveRecorderLogic->getFocusCurveType());
        //
        // Will take place for each newly measured data point. Each new data point will be added to focusCurveRecordSet before.
        // Now the difference is, that this time the fitting curve (which is used inside) actually has some fixed parameters (the ones we know from the long measurements...)
        // So, those numbers have to be passed somehow to the FittingCurveT function object (which is currently located even inside the fit() function of the CurveFitAlgorithmT)....
        // -> Question is, if FocusCurveT should take a FittingCurveT object ptr instead of an enum type.....?! Then it would be possible to write:
        //
        // std::shared_ptr<LmFittingCurveT> fc = LmFittingCurveFactoryT::getInstance(ct);
        // fc->fixParm("A_IDX", value);
        //
        // Even worse: It depends on the curve type which parameters need to be fixed! This "knowledge" should not be coded into the LmFittingCurveT sub-classes, because it is a "special thing" regarding the algorithm which wants to fix certain curve parameters to certain values.
        // Idea: "keepShape" or "positionParmsOnly" could be abstractions...
        // Maybe I should first test if fixing a and b of the hyperbolc curve is even successful... (-> write a new sub-class of FittingCurveT, and somehow hach passing in the two parameters (just hardcode them!))... If that really works, the effort to generalize the fixing of curve parameters is probably worth it! But not before!


        // HACK TO TEST THE CONCEPT

        // // ESTIMATE THE CLOSEST BOUNDARY USING THE KNOWN FOCUS CURVE
        // // Calc start pos. (average over two points)
        // float xLimitRel = focusCurveFunction->f_inv(focusMeasureLimit); // NOTE: Should always return the positive part (TODO: May fail...)

        // float focusMeasure1 = selfOrientationResult.record1->getFocusMeasure(curveFocusMeasureType);
        // float absPos1 = selfOrientationResult.record1->getCurrentAbsoluteFocusPos();
        // float relEstimatedPos1 = focusCurveFunction->f_inv(focusMeasure1);

        // float focusMeasure2 = selfOrientationResult.record2->getFocusMeasure(curveFocusMeasureType);
        // float absPos2 = selfOrientationResult.record2->getCurrentAbsoluteFocusPos();
        // float relEstimatedPos2 = focusCurveFunction->f_inv(focusMeasure2);

        // float deltaReallyMoved = absPos1 - absPos2;

        // float distToLimit1 = xLimitRel - relEstimatedPos1;
        // float distToLimit2 = xLimitRel - relEstimatedPos2;

        // float currAbsPos = getFocusController()->getFocusInterface()->getCurrentPos();

        // // Average out the error in the xrel (resulting from moise in the focus measures) which implicitly lead to estimation/lookup errors (xrel).
        // float dxRel1;
        // float dxRel2;

        // if (curveHalf == CurveSectorT::LEFT) {
        //   // x2 > x1 -> distToLimit2 < distToLimit1, deltaReallyMoved > 0 since moving always INWARD
        //   dxRel1 = distToLimit1 - deltaReallyMoved;
        //   dxRel2 = distToLimit2;
        // } else {
        //   // X2 < x1 -> distToLimit2 > distToLimit1, deltaReallyMoved > 0 since moving always INWARD
        //   dxRel1 = distToLimit1;
        //   dxRel2 = distToLimit2 - deltaReallyMoved;
        // }

        // // Estimated, averaged relative distance from current focus pos. to closest limit.
        // float dxRelAv = (dxRel1 + dxRel2) / 2.0; // TODO: What if dxRelAv < 0?

        // float minTheoreticalFocusMeasure = focusCurveFunction->f(0);

        // LOG(debug) << "xLimitRel=" << xLimitRel << std::endl
        // 	       << "Measure 1 (absPos=" << absPos1 << ", focusMeasure=" << focusMeasure1 << ", relEstimatedPos=" << relEstimatedPos1 << ")" << std::endl
        // 	       << "Measure 2 (absPos=" << absPos2 << ", focusMeasure=" << focusMeasure2 << ", relEstimatedPos=" << relEstimatedPos2 << ")" << std::endl
        // 	       << "deltaReallyMoved=" << deltaReallyMoved << std::endl
        // 	       << "distToLimit1=" << distToLimit1 << std::endl
        // 	       << "distToLimit2=" << distToLimit2 << std::endl
        // 	       << "currAbsPos=" << currAbsPos << std::endl
        // 	       << "absDest=" << (selfOrientationResult.focusDirectionToLimit == FocusDirectionT::INWARD ? currAbsPos + dxRelAv : currAbsPos - dxRelAv) << std::endl
        // 	       << "dxRel1=" << dxRel1 << ", dxRel2=" << dxRel2 << ", dxRelAv=" << dxRelAv << std::endl
        // 	       << "direction to limit: " << FocusDirectionT::asStr(selfOrientationResult.focusDirectionToLimit) << std::endl
        // 	       << "minTheoreticalFocusMeasure: " << minTheoreticalFocusMeasure << std::endl;

        // // Invert movement direction if negative (HACK?!)
        // FocusDirectionT::TypeE finalDir = selfOrientationResult.focusDirectionToLimit;

        // if (dxRelAv > 0) {
        //   finalDir = (finalDir == FocusDirectionT::INWARD ? FocusDirectionT::OUTWARD : FocusDirectionT::INWARD);
        // }

        // getFocusController()->moveFocusByBlocking(finalDir, std::fabs(dxRelAv), 30000ms);






        // TODO: Is it even required to store the return value...?
        /*int closerAbsBoundaryPos = */
        getFocusController()->boundaryScanWithFocusCurveSupport(focusCurveFunction,
                                                                                           selfOrientationResult,
                                                                                           curveFocusMeasureType,
                                                                                           focusMeasureLimit,
                                                                                           1.5F /*focusMeasureDelta - TODO: Do not hardcode */);




        //
        //
        //
        //
        // With the boundary info - record a curve and do the curve match every round...
        //
        //
        //
        // GIVEN: -closerAbsBoundaryPos (current pos.)
        //        -curveHalf
        //TODO: REMOVE THIS LINE    ----------------------- WORKING UNTIL HERE --------------------------------

        // TODO: Is it possible to estimate the opposite boundary? -> yes, at least the minimal distance...
        //int estimatedAbsOppositeBoundary = ;
        float minDistToMove = 10000; // TODO: Roughly estimate distance to other boundary... (min distance).


        float startAbsPos = (float) getFocusController()->getFocus()->getCurrentPos();
        float currentAbsPos;
        float stepsMovedSoFar;

        std::shared_ptr<FocusCurveT> focusCurve = nullptr;
        float focusMeasure;
        bool endRecording;

        do {
            // Movement direction is opposite direction than direction to boundary
            //-> NO! Not always!!!
            //FocusDirectionT::TypeE dirToOtherBoundary = FocusDirectionT::invert(selfOrientationResult.focusDirectionToLimit);
            // TODO: Is this ok now??
            FocusDirectionT::TypeE dirToOtherBoundary = (curveHalf == CurveSectorT::LEFT ? FocusDirectionT::OUTWARD
                                                                                         : FocusDirectionT::INWARD);

            getFocusController()->moveFocusByBlocking(dirToOtherBoundary, foFiStepSize, 30000ms);

            stepsMovedSoFar = std::fabs(currentAbsPos - startAbsPos);

            auto newRecord = getFocusController()->measureFocus();
            records->push_back(newRecord);

            focusMeasure = newRecord->getFocusMeasure(curveFocusMeasureType);

            currentAbsPos = (float) getFocusController()->getFocus()->getCurrentPos();


            throw BaseExceptionT("NOT IMPLEMENTED");

            try {
                // TODO / HACK / FIXME: Does not compile
                //focusCurve = std::make_shared<FocusCurveT>(records, FittingCurveTypeT::HYPERBOLIC_POS_ONLY);
                // HACK! FIXME! Just to make it compile...
                focusCurve = nullptr;


                float estimatedBestAbsPos = focusCurve->getCurveParms().get("C_IDX").getValue();

                LOG(debug)
                    << "SinglePassFocusingStrategyT::run...curve matching successful... currently best focus pos: "
                    << estimatedBestAbsPos << std::endl;

            } catch (CurveFitExceptionT &exc) {
                LOG(debug) << "SinglePassFocusingStrategyT::run...curve matching failed... " << exc.what()
                           << ". Retrying..." << std::endl;
            }

            //TODO: Print exit conditions...
            bool focusLimitReached = (focusMeasure >= focusMeasureLimit);
            bool movedMinimumDistance = (stepsMovedSoFar >= minDistToMove);

            endRecording = (movedMinimumDistance && focusLimitReached);

            LOG(debug) << "SinglePassFocusingStrategyT::run..."
                       << "Checking loop end condition: focusLimitReached=" << focusLimitReached
                       << ", movedMinimumDistance=" << movedMinimumDistance
                       << ", -> endRecording=" << endRecording << std::endl;

        } while (!endRecording); // end while


        if (focusCurve != nullptr) {
            float estimatedBestAbsPosFinal = focusCurve->getCurveParms().get("C_IDX").getValue();
            LOG(debug) << "SinglePassFocusingStrategyT::run... - Finally best focus position (estimated)="
                       << estimatedBestAbsPosFinal << std::endl;

            getFocusController()->moveFocusToBlocking((int) estimatedBestAbsPosFinal, 30000ms);

            auto finalRecord = getFocusController()->measureFocus();
            float currFocusMeasure = finalRecord->getFocusMeasure(curveFocusMeasureType);

            LOG(debug) << "SinglePassFocusingStrategyT::run... - FINAL, currFocusMeasure=" << currFocusMeasure
                       << std::endl;

        } else {
            LOG(error) << "SinglePassFocusingStrategyT::run... - Best focus position not determined..." << std::endl;
            // TODO: ERROR / throw? / notify failed....?
        }








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

void AveragedFocsCurveFocusingStrategyT::rollbackFocus() {
    using namespace std::chrono_literals;

    LOG(debug)
        << "AveragedFocsCurveFocusingStrategyT::rollbackFocus..." << std::endl;

    getFocusController()->moveFocusToBlocking(mInitialAbsPosition, 30000ms);
}

void AveragedFocsCurveFocusingStrategyT::focusFinderCleanup() {
    LOG(debug)
        << "AveragedFocsCurveFocusingStrategyT::focusFinderCleanup..." << std::endl;

    rollbackFocus();
}

void AveragedFocsCurveFocusingStrategyT::cancel() {
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
