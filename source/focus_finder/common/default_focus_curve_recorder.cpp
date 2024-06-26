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

#include <chrono>

#include <boost/bind/bind.hpp>
#include <utility>

#include "include/default_focus_curve_recorder.h"
#include "include/logging.h"
#include "include/timeout_exception.h"

#include "include/focus_measure_type.h"
#include "include/focus_curve_record.h"
#include "include/focus_curve_record_set.h"
#include "include/curve_fit_algorithm.h"
#include "include/curve_sector.h"
#include "include/focus_direction.h"

DefaultFocusCurveRecorderT::DefaultFocusCurveRecorderT(std::shared_ptr<FocusControllerT> focusController)
        : FocusCurveRecorderT(std::move(focusController)),
          mCancelled(false),
          mIsRunning(false) {
    using namespace std::placeholders;

    LOG(debug)
        << "DefaultFocusCurveRecorderT::DefaultFocusCurveRecorderT..." << std::endl;

    mFocusCurveRecordSets = std::make_shared<FocusCurveRecordSetContainerT>();


    // Hand focus controller events through
    getFocusController()->registerFocusControllerProgressUpdateListener(
            [this](float progress,
                   const std::string &msg,
                   std::shared_ptr<FocusCurveRecordT> focusCurveRecord) { onFocusControllerProgressUpdate(progress, msg, std::move(focusCurveRecord)); }
    );
    getFocusController()->registerFocusControllerNewRecordListener(
            [this](std::shared_ptr<FocusCurveRecordT> focusCurveRecord) { onFocusControllerNewRecord(std::move(focusCurveRecord)); }
    );
}

void DefaultFocusCurveRecorderT::onFocusControllerProgressUpdate(float progress,
                                                                 const std::string &msg,
                                                                 std::shared_ptr<FocusCurveRecordT> focusCurveRecord) {
    LOG(debug)
        << "DefaultFocusCurveRecorderT::onFocusControllerProgressUpdate... handing through FocusControllerProgressUpdate..."
        << std::endl;

    notifyFocusCurveRecorderProgressUpdate(progress, msg, std::move(focusCurveRecord));
}

void DefaultFocusCurveRecorderT::onFocusControllerNewRecord(std::shared_ptr<FocusCurveRecordT> focusCurveRecord) {
    LOG(debug)
        << "DefaultFocusCurveRecorderT::onFocusControllerNewRecord... handing through FocusControllerNewRecord..."
        << std::endl;

    notifyFocusCurveRecorderNewRecord(std::move(focusCurveRecord));
}


std::string DefaultFocusCurveRecorderT::getName() const {
    return "DefaultFocusCurveRecorderT";
}

void DefaultFocusCurveRecorderT::reset() {

    // TODO: What happens if called while running? -> Exception?
    LOG(debug)
        << "DefaultFocusCurveRecorderT::reset..." << std::endl;

    mFocusCurveRecordSets->clear();


    getFocusController()->cleanup();
}

void DefaultFocusCurveRecorderT::checkCancelled() const {
    if (mCancelled.load()) {
        throw FocusControllerCancelledExceptionT("Focus curve recorder cancelled.");
    }
}

FocusMeasureTypeT::TypeE DefaultFocusCurveRecorderT::getLimitFocusMeasureType() const {
    return getFocusController()->getFocusFinderProfile().getLimitFocusMeasureType();
}


FocusMeasureTypeT::TypeE DefaultFocusCurveRecorderT::getCurveFocusMeasureType() const {
    return getFocusController()->getFocusFinderProfile().getCurveFocusMeasureType();
}


float DefaultFocusCurveRecorderT::getFocusMeasureLimit() const {
    return getFocusController()->getFocusFinderProfile().getFocusMeasureLimit();
}

int DefaultFocusCurveRecorderT::getStepSize() const {
    return getFocusController()->getFocusFinderProfile().getStepSize();
}


CurveSectorT::TypeE DefaultFocusCurveRecorderT::locateStartingPosition() {
    LOG(debug)
        << "DefaultFocusCurveRecorderT::locateStartingPosition..." << std::endl;

    SelfOrientationResultT selfOrientationResult = getFocusController()->performSelfOrientation(getFocusMeasureLimit());

    // Move close to the limiting focus measure (into the determined direction)
    getFocusController()->boundaryScanLinear(selfOrientationResult, (float) getStepSize(), getFocusMeasureLimit());

    return selfOrientationResult.curveHalf;
}


std::shared_ptr<FocusCurveRecordSetT>
DefaultFocusCurveRecorderT::recordFocusCurveRecordSet(CurveSectorT::TypeE curveHalf) {
    using namespace std::chrono_literals;

    LOG(debug)
        << "DefaultFocusCurveRecorderT::recordFocusCheckpoints..." << std::endl;

    auto focusCurveRecordSet = std::make_shared<FocusCurveRecordSetT>(getCurveFocusMeasureType(),
                                                                      getFocusMeasureLimit());

    // Store recorded focus curve record set
    mFocusCurveRecordSets->push_back(focusCurveRecordSet);


    FocusDirectionT::TypeE recordingDirection = (curveHalf == CurveSectorT::LEFT ? FocusDirectionT::OUTWARD
                                                                                 : FocusDirectionT::INWARD);

    LOG(debug) << "We are on the " << CurveSectorT::asStr(curveHalf) << "... -> recording direction="
               << FocusDirectionT::asStr(recordingDirection) << std::endl;

    auto curveRecord = getFocusController()->measureFocus();

    focusCurveRecordSet->push_back(curveRecord);

    // Notify about FocusCurve recorder update...
    notifyFocusCurveRecorderProgressUpdate(0, "Starting to record curve...", curveRecord);

    bool continueRecording;

    do {
        notifyFocusCurveRecorderRecordSetUpdate(focusCurveRecordSet);

        getFocusController()->moveFocusByBlocking(recordingDirection, getStepSize(), 30000ms);

        notifyFocusCurveRecorderProgressUpdate(-1, "Recording curve...", curveRecord);

        curveRecord = getFocusController()->measureFocus();

        focusCurveRecordSet->push_back(curveRecord);

        // Notify about FocusCurve recorder update...
        //notifyFocusCurveRecorderProgressUpdate(-1, "Recording curve...", curveRecord);
        //notifyFocusCurveRecorderRecordSetUpdate(focusCurveRecordSet);

        checkCancelled();

        continueRecording = (focusCurveRecordSet->size() < 3 ||
                             curveRecord->getFocusMeasure(getLimitFocusMeasureType()) < getFocusMeasureLimit());

        LOG(debug) << "DefaultFocusCurveRecorderT::recordFocusCurveRecordSet - getLimitFocusMeasureType()="
                   << FocusMeasureTypeT::asStr(getLimitFocusMeasureType())
                   << ", focusMeasure=" << curveRecord->getFocusMeasure(getLimitFocusMeasureType())
                   << ", focusMeasureLimit=" << getFocusMeasureLimit()
                   << ", continueRecording=" << continueRecording << std::endl;

        // Just avoid a stop for the first 2 measurements in case the focus measure limit condition may not be true because of noise.
    } while (continueRecording);

    notifyFocusCurveRecorderProgressUpdate(100, "Finished recording curve.");

    LOG(debug) << *focusCurveRecordSet << std::endl;

    return focusCurveRecordSet;
}


bool DefaultFocusCurveRecorderT::isRunning() const {
    return mIsRunning.load();
}

void DefaultFocusCurveRecorderT::run() {
    mIsRunning = true;

    LOG(debug)
        << "DefaultFocusCurveRecorderT::run..." << std::endl;


    try {
        // 	using namespace std::chrono_literals;

        // 	// TODO: Check that device manager is set

        // 	// TODO: filter...

        // NOTE: Moved to focus_analyzer.cpp constructor
        // // Register on camera
        // // NOTE / TODO / FIXME: For some reason std::bind did not compile....
        // mCameraExposureFinishedConnection =
        //   getCameraInterface()->registerExposureCycleFinishedListener(
        // 							 std::bind(&DefaultFocusCurveRecorderT::onImageReceived,
        // 								     this, _1, _2, _3));


        // Notify that focus finder started
        notifyFocusCurveRecorderStarted();

        getFocusController()->devicesAvailabilityCheck();


        CurveSectorT::TypeE curveHalf = locateStartingPosition();

// while() {
        // TODO: Need loop to record multiple focus curve record sets...
        auto focusCurveRecordSet = recordFocusCurveRecordSet(curveHalf); // std::shared_ptr<FocusCurveRecordSetT>


        notifyFocusCurveRecorderRecordSetFinished(focusCurveRecordSet);
//}



        // 	// First, only simulate a long delay....

        // 	LOG(debug)
        // 	<< "DefaultFocusCurveRecorderT::run - phase 1..." << std::endl;

        // 	// Notify about FoFi update...
        // 	notifyFocusFinderProgressUpdate(0.0, "Starting...");

        // 	// TODO: Set the filter...


        // 	// TODO / HACK....
        // 	for(int i = 0; i < 5; ++i) {

        // 		// TODO: First take picture, then move... -> move down.....?!!!!
        // 		moveFocusByBlocking(FocusDirectionT::INWARD, 1000, 30000ms);

        // 		// This is the focus position for which a star image wil be taken
        // 		int currentFocusPos = getFocusInterface()->getCurrentPos();

        // 		PointT<unsigned int> lastFocusStarPos = getLastFocusStarPos().to<
        // 				unsigned int>();

        // 		LOG(debug)
        // 		<< "DefaultFocusCurveRecorderT::run - focus star pos=("
        // 				<< lastFocusStarPos.x() << ", " << lastFocusStarPos.y() << ")"
        // 				<< std::endl;

        // 		// Calc "extended ROI (we want some more space around the actual star window size -> factor).
        // 		// TODO: Simplify: Overload operator=* -> multiply scalar with size.
        // 		// NOTE: +1 to make the result odd...
        // 		unsigned int factor = 2;
        // 		SizeT<unsigned int> windowSize =
        // 				getFocusFinderProfile().getStarWindowSize();

        // 		// NOTE: In case factor is evenm we need to add +1 to make the result odd.
        // 		int oddMaker = (factor % 2 == 0 ? +1 : 0);

        // 		SizeT<unsigned int> outerWindowSize(
        // 				windowSize.width() * factor + oddMaker,
        // 				windowSize.height() * factor + oddMaker);

        // 		// NOTE: This is in frame coordinates
        // 		auto outerRoi = RectT<unsigned int>::fromCenterPoint(lastFocusStarPos,
        // 				outerWindowSize);

        // 		LOG(debug)
        // 		<< "DefaultFocusCurveRecorderT::run - calculated outer ROI=" << outerRoi
        // 				<< std::endl;

        // 		// Set ROI based on last focus star position
        // 		getCameraInterface()->setRoi(outerRoi);

        // 		// Take a picture
        // 		runExposureBlocking(1000ms /*TODO: Set exp time*/);

        // 		// Calc center of subframe
        // 		PointT<unsigned int> centerOuterSubframe(
        // 				std::ceil(mCurrentImage->width() / 2),
        // 				std::ceil(mCurrentImage->height() / 2));

        // 		LOG(debug)
        // 		<< "DefaultFocusCurveRecorderT::run - centerOuterSubframe="
        // 				<< centerOuterSubframe << std::endl;

        // 		// In outer subframe (mCurrentImage) coordinates
        // 		auto innerRoi = RectT<unsigned int>::fromCenterPoint(
        // 				centerOuterSubframe,
        // 				getFocusFinderProfile().getStarWindowSize());

        // 		LOG(debug)
        // 		<< "DefaultFocusCurveRecorderT::run - calculated inner ROI=" << innerRoi
        // 				<< std::endl;

        // 		// get_crop() from mCurrentImage using innerRoi
        // 		ImageT innerSubFrameImg = mCurrentImage->get_crop(innerRoi.x() /*x0*/,
        // 				innerRoi.y() /*y0*/, innerRoi.x() + innerRoi.width() - 1/*x1*/,
        // 				innerRoi.y() + innerRoi.height() - 1/*y1*/
        // 				);

        // 		LOG(debug)
        // 		<< "DefaultFocusCurveRecorderT::run - innerSubFrameImg size - w="
        // 				<< innerSubFrameImg.width() << ", h="
        // 				<< innerSubFrameImg.height() << std::endl;

        // 		// Determine SNR of recorded area to check if there could be astar around.
        // 		checkIfStarIsThere(innerSubFrameImg);

        // 		// TODO: Do not hardcode IWC, do not hardcode "sub mean"
        // 		ImageT newCenterImage;

        // 		auto newCentroidOpt = CentroidT::calculate(innerSubFrameImg,
        // 				CentroidTypeT::IWC, true /*sub mean*/, &newCenterImage);

        // 		if (!newCentroidOpt) {
        // 			// TODO: Improve logging? ReportingT?
        // 			LOG(error)
        // 			<< "Unable to determine new centroid." << std::endl;

        // 			// TODO: Retry? Or just cancel? For now we cancel...
        // 			throw FocusFinderCancelledExceptionT();
        // 		}

        // 		// NOTE: Those coordinates are in coordinates of the inner subframe! We need them in "outer sub frame" coordinates!
        // 		int deltaX = (outerRoi.width() - innerRoi.width()) / 2;
        // 		int deltaY = (outerRoi.height() - innerRoi.height()) / 2;

        // 		LOG(debug)
        // 		<< "DefaultFocusCurveRecorderT::run - deltaX=" << deltaX << ", deltaY="
        // 				<< deltaY << std::endl;

        // 		PointFT newCentroidInnerRoiCoords = *newCentroidOpt;
        // 		PointFT newCentroidOuterRoiCoords(
        // 				newCentroidInnerRoiCoords.x() + deltaX,
        // 				newCentroidInnerRoiCoords.y() + deltaY);

        // 		LOG(debug)
        // 		<< "DefaultFocusCurveRecorderT::run - new centroid (inner frame)="
        // 				<< newCentroidInnerRoiCoords << std::endl;
        // 		LOG(debug)
        // 		<< "DefaultFocusCurveRecorderT::run - new centroid (outer frame)="
        // 				<< newCentroidOuterRoiCoords << std::endl;

        // 		// Use newly calculated center (newCentroidOuterRoiCoords) to again get sub-frame from mCurrentImage
        // 		// TODO: Currently we just round() the float position to the closest int. This may be
        // 		//       improved by introducing sub-pixel accuracy...
        // 		// TODO: We may introduce a round() function for conversion from PointTF to PointT...?
        // 		PointT<unsigned int> newCentroidRoundedToNextInt(
        // 				std::round(newCentroidOuterRoiCoords.x()),
        // 				std::round(newCentroidOuterRoiCoords.y()));

        // 		LOG(debug)
        // 		<< "DefaultFocusCurveRecorderT::run - new centroid rounded to next int="
        // 				<< newCentroidRoundedToNextInt << std::endl;

        // 		auto innerCorrectedRoi = RectT<unsigned int>::fromCenterPoint(
        // 				newCentroidRoundedToNextInt,
        // 				getFocusFinderProfile().getStarWindowSize());

        // 		LOG(debug)
        // 		<< "DefaultFocusCurveRecorderT::run - calculated inner corrected ROI="
        // 				<< innerCorrectedRoi << std::endl;

        // 		// get_crop() from mCurrentImage using corrected innerRoi
        // 		ImageT innerCorrectedSubFrameImg = mCurrentImage->get_crop(
        // 				innerCorrectedRoi.x() /*x0*/, innerCorrectedRoi.y() /*y0*/,
        // 				innerCorrectedRoi.x() + innerCorrectedRoi.width() - 1/*x1*/,
        // 				innerCorrectedRoi.y() + innerCorrectedRoi.height() - 1/*y1*/
        // 				);

        // 		LOG(debug)
        // 		<< "DefaultFocusCurveRecorderT::run - innerCorrectedSubFrameImgsize - w="
        // 				<< innerCorrectedSubFrameImg.width() << ", h="
        // 				<< innerCorrectedSubFrameImg.height() << std::endl;

        // 		// Determine SNR of recorded area to check if there could be a star around.
        // 		float snr;
        // 		checkIfStarIsThere(innerCorrectedSubFrameImg, &snr);

        // 		// TODO: Calculate HFD
        // 		HfdT hfd(innerCorrectedSubFrameImg);
        // 		LOG(debug)
        // 		<< "DefaultFocusCurveRecorderT::run - HFD: " << hfd.getValue() << std::endl;

        // 		// NOTE / TODO: This only works ifh height() is odd
        // 		size_t centerIdxHorz = std::floor(
        // 				innerCorrectedSubFrameImg.height() / 2);

        // 		FwhmT fwhmHorz(
        // 				ImageSlicerT::slice<SliceDirectionT::HORZ>(
        // 						innerCorrectedSubFrameImg, centerIdxHorz));

        // 		// NOTE / TODO: This only works if width() is odd
        // 		size_t centerIdxVert = std::floor(
        // 				innerCorrectedSubFrameImg.width() / 2);

        // 		FwhmT fwhmVert(
        // 				ImageSlicerT::slice<SliceDirectionT::VERT>(
        // 						innerCorrectedSubFrameImg, centerIdxVert));

        // 		LOG(debug)
        // 		<< "DefaultFocusCurveRecorderT::run - FWHM(HORZ): " << fwhmHorz.getValue()
        // 				<< ", FWHM(VERT): " << fwhmVert.getValue() << std::endl;

        // 		// Convert center coordinates to frame coordinates (absolute position within image)
        // 		PointFT newCentroidAbsRoiCoords(
        // 				newCentroidOuterRoiCoords.x() + outerRoi.x(),
        // 				newCentroidOuterRoiCoords.y() + outerRoi.y());

        // 		// Calculate "drift" (movement of center position since last picture
        // 		std::tuple<float, float> drift(
        // 				newCentroidAbsRoiCoords.x() - lastFocusStarPos.x(),
        // 				newCentroidAbsRoiCoords.y() - lastFocusStarPos.y());

        // 		// Fill all so far collected data into the "FoFi Result Structure"
        // 		auto record = FocusFinderRecordBuilderT()
        // 				.setAbsoluteFocusPos(currentFocusPos)
        // 				.setDrift(drift)
        // 				.setSnr(snr)
        // 				.setHorzFwhm(fwhmHorz)
        // 				.setVertFwhm(fwhmVert)
        // 				.setHfd(hfd)
        // 				.setCorrectedStarImage(innerCorrectedSubFrameImg)
        // 				.build();

        // 		// Notify about FoFi update...
        // 		notifyFocusFinderProgressUpdate(60.0, "Phase 2 finished.", record);
        // }










        // 	using namespace std::chrono_literals;
        // 	std::this_thread::sleep_for(2s);

        // 	// Check if it was cancelled
        // 	checkCancelled();

        // 	// Notify about FoFi update...
        // 	notifyFocusFinderProgressUpdate(30.0, "Phase 1 finished.");

        // 	LOG(debug)
        // 	<< "DefaultFocusCurveRecorderT::run - phase 2..." << std::endl;

        // 	std::this_thread::sleep_for(2s);

        // 	// Check if it was cancelled
        // 	checkCancelled();

        // 	// Notify about FoFi update...
        // 	notifyFocusFinderProgressUpdate(60.0, "Phase 2 finished.");

        // 	LOG(debug)
        // 	<< "DefaultFocusCurveRecorderT::run - phase 3..." << std::endl;

        // 	std::this_thread::sleep_for(2s);

        // 	// Check if it was cancelled
        // 	checkCancelled();

        // 	// HACK!!! TODO: Remove...
        // 	// MyDataContainerT dataPointsToFit;
        // 	// dataPointsToFit.push_back(std::make_pair<float, float>(1, 2));
        // 	// dataPointsToFit.push_back(std::make_pair<float, float>(3, 4));
        // 	// dataPointsToFit.push_back(std::make_pair<float, float>(5, 6));
        // 	// dataPointsToFit.push_back(std::make_pair<float, float>(6, 4));
        // 	// dataPointsToFit.push_back(std::make_pair<float, float>(7, 3));
        // 	// dataPointsToFit.push_back(std::make_pair<float, float>(8, 2));

        // 	// // TODO: See FwhmT for usage....
        // 	// CurveParmsT p = CurveFitAlgorithmT::fitCurve(FittingCurveTypeT::TypeE ct, Rng dps,
        // 	// 	const CurveFitParmsT & curveFitParms,
        // 	// 					     CurveFitSummaryT * outSummary = nullptr);


        // 	// TODO: The hyperbolic fit will not be performed here... only after measurement... / at the end...
        // 	// CurveParamsT::TypeT hyperbolCurveParms;
        // 	// HyperbolMatcherT::fitGslLevenbergMarquart<MyDataAccessorT>(
        // 	// 		dataPointsToFit, &hyperbolCurveParms, 0.1f /*EpsAbs*/,
        // 	// 		0.1f /*EpsRel*/);

        // 	// // Print calculated curve parameters
        // 	// LOG(debug)
        // 	// << "a=" << hyperbolCurveParms[CurveParamsT::A_IDX] << std::endl << "b="
        // 	// 		<< hyperbolCurveParms[CurveParamsT::B_IDX] << std::endl << "c="
        // 	// 		<< hyperbolCurveParms[CurveParamsT::C_IDX] << std::endl << "d="
        // 	// 		<< hyperbolCurveParms[CurveParamsT::D_IDX] << std::endl;

        // 	// LOG(debug)
        // 	// << "DefaultFocusCurveRecorderT::run - finished" << std::endl;

        // 	// Notify about FoFi update...
        // 	notifyFocusFinderProgressUpdate(100.0, "Phase 3 finished.");

        // 	// Notify that curve recorder is finished...
        notifyFocusCurveRecorderFinished(mFocusCurveRecordSets);


        getFocusController()->cleanup();


    } catch (FocusControllerFailedExceptionT &exc) {
        // TODO: Improve error handling... -> ReportingT?
        LOG(warning)
            << "Focus curve recorder failed." << std::endl;

        // TODO: Problem... cleanup also can take time since focus position may be changed... -> introduce additional state "cancelling"?
        getFocusController()->cleanup();

        // Notify that FocusCurveRecorder was cancelled...
        // TODO: Introduce notifyFocusCurveRecorderFailed()....
        notifyFocusCurveRecorderCancelled();
    }
    catch (FocusControllerCancelledExceptionT &exc) {

        // TODO: Improve error handling... -> ReportingT?
        LOG(warning)
            << "Focus curve recorder was cancelled." << std::endl;

        // TODO: Problem... cleanup also can take time since focus position may be changed... -> introduce additional state "cancelling"?
        getFocusController()->cleanup();

        // Notify that FocusCurveRecorder was cancelled...
        notifyFocusCurveRecorderCancelled();
    }
        // catch (FocusExceptionT & exc) {
        // 	// TODO: MAybe write one common exception handler for the FoFi here since code repeats!
        // 	focusFinderCleanup();

        // 	// TODO: Improve error handling... -> ReportingT?
        // 	LOG(error)
        // 	<< "Hit focus exception: " << exc.what() << std::endl;

        // 	// Notify that FoFi was cancelled...
        // 	// TODO: Maybe introduce notifyFocusFinderFailed()...
        // 	notifyFocusFinderCancelled();
        //}
    catch (TimeoutExceptionT &exc) {
        // TODO: Improve error handling... -> ReportingT?
        LOG(error)
            << "Hit timeout, unable tu finish curve recording." << std::endl;

        getFocusController()->cleanup();

        // Notify that curve recorder was cancelled...
        // TODO: Maybe introduce notifyFocusFinderFailed()...
        notifyFocusCurveRecorderCancelled();
    }
    catch (CurveFitExceptionT &exc) {
        // TODO: This should be caught already earlier and should not cancel the entire FocusCurveRecorder
        LOG(warning)
            << "Matching the focus curve failed." << std::endl;

        getFocusController()->cleanup();
    }

    mIsRunning = false;
}

// TODO: Remove... moved to focus_analyzer.cpp
// void DefaultFocusCurveRecorderT::cleanup() {
//   using namespace std::chrono_literals;

//   LOG(debug)
//     << "DefaultFocusCurveRecorderT::cleanup..." << std::endl;

//   // In any case move the focus to the initial position (if focus device is still available)
//   try {
//     moveFocusToBlocking(mInitialFocusPos, 30000ms /*timeout*/);
//   } catch (FocusCurveRecorderFailedExceptionT & exc) {
//     LOG(error) << "DefaultFocusCurveRecorderT::cleanup... no focus device set!" << std::endl;
//   }

//   getCameraInterface()->unregisterExposureCycleFinishedListener(mCameraExposureFinishedConnection);
// }

void DefaultFocusCurveRecorderT::cancel() {
    // mCancelled = true;
    // cv.notify_all();

    mCancelled = true;
    getFocusController()->cancel();
}

std::shared_ptr<const FocusCurveRecordSetContainerT> DefaultFocusCurveRecorderT::getFocusCurveRecordSets() const {
    return mFocusCurveRecordSets;
}
