#include <thread> // TODO: Remove - only temporary
#include <tuple>
#include <chrono>

#include "include/focus_finder_fast_curve_lookup.h"
#include "include/logging.h"
#include "include/exception.h"
#include "include/wait_for.h"

#include "include/snr.h"
#include "include/centroid.h"
#include "include/hfd.h"
#include "include/fwhm.h"
#include "include/image_slicer.h"
#include "include/focus_curve_record.h"
#include "include/focus_curve_record_builder.h"

#include "include/camera.h"
#include "include/focus.h"
#include "include/filter.h"

// TODO: Move tp parent class?!
DEF_Exception(FocusFinderCancelled);

FocusFinderFastCurveLookupT::FocusFinderFastCurveLookupT() :
		mCancelled(false),
        mIsRunning(false) {
	LOG(debug)
	<< "FocusFinderFastCurveLookupT::FocusFinderFastCurveLookupT..." << std::endl;
}

std::string FocusFinderFastCurveLookupT::getName() const {
	return "FocusFinderFastCurveLookupT";
}

void FocusFinderFastCurveLookupT::reset() {
	// TODO
	// TODO: What happens if called while running? -> Exception?
	LOG(debug)
	<< "FocusFinderFastCurveLookupT::reset..." << std::endl;
}

void FocusFinderFastCurveLookupT::checkCancelled() const {
	if (mCancelled.load()) {
		throw FocusFinderCancelledExceptionT("Focus finder cancelled.");
	}
}

void FocusFinderFastCurveLookupT::moveFocusByBlocking(
		FocusDirectionT::TypeE direction, int ticks,
		std::chrono::milliseconds timeout) {

	auto isFocusPositionReachedOrCancelledLambda = [this]() -> bool {
		LOG(debug) << "current pos=" << getFocus()->getCurrentPos()
		<< ", target pos=" << getFocus()->getTargetPos()
		<< ", isMoving? " << getFocus()->isMoving()
		<< std::endl;

		// TODO: focus->getTargetPos() currently is not implemented..
		//       not sure if target and/or current position are maintained by INDI focus driver...
		//return (focus->getCurrentPos() == focus->getTargetPos() && ! focus->isMoving());
			return (! getFocus()->isMoving() || mCancelled.load());
		};

	// Set the target position (rel)
	getFocus()->setTargetPos(ticks, direction);

	wait_for(isFocusPositionReachedOrCancelledLambda, timeout);

	// If it was cancelled, throw cancel exception
	if (mCancelled) {
		throw FocusFinderCancelledExceptionT();
	}
}

// NOTE: Callback from camera device, registered in setCamera of base class FocusFinder
void FocusFinderFastCurveLookupT::onImageReceived(RectT<unsigned int> roi,
		std::shared_ptr<const ImageT> image, bool lastFrame) {

	// TODO: Store image.... roi etc...
	mCurrentImage = image;

	cv.notify_all();

}

// TODO: Add binning?
void FocusFinderFastCurveLookupT::runExposureBlocking(
		std::chrono::milliseconds expTime) {

	using namespace std::chrono_literals;

//	TODO: try catch?

	// Start exposure
	getCamera()->setExposureTime(1000ms);
	getCamera()->startExposure();

	// Use condition variable to wait for finished exposure.
	std::unique_lock<std::mutex> lk(cvMutex);
	cv.wait(lk);

	// If it was cancelled, throw cancel exception
	if (mCancelled) {
		throw FocusFinderCancelledExceptionT();
	}
}

void FocusFinderFastCurveLookupT::checkIfStarIsThere(const ImageT & img,
		float * outSnr) const {
	float snr = SnrT::calculate(img);

	LOG(debug)
	<< "FocusFinderFastCurveLookupT::run - SNR: " << snr << std::endl;

	if (snr < getFocusFinderProfile().getStarDetectionSnrBoundary()) {
		LOG(warning)
		<< "FocusFinderFastCurveLookupT::run - no valid star detected." << std::endl;

		// TODO: How to handle? Just cancel? Or repeat? For now we cancel...
		throw FocusFinderCancelledExceptionT();
	}

	if (outSnr != nullptr) {
		*outSnr = snr;
	}
}

bool FocusFinderFastCurveLookupT::isRunning() const {
    return mIsRunning.load();
}

void FocusFinderFastCurveLookupT::run() {
    mIsRunning = true;

	LOG(debug)
	<< "FocusFinderFastCurveLookupT::run..." << std::endl;

	try {
		using namespace std::chrono_literals;

		// TODO: Check that device manager is set

		// TODO: filter...

		// TODO: Check that camera is there
		// TODO: Check that focus is there

		// Register on camera
		// NOTE / TODO: For some reason std::bind did not compile....
		mCameraExposureFinishedConnection =
				getCamera()->registerExposureCycleFinishedListener(
						boost::bind(&FocusFinderFastCurveLookupT::onImageReceived,
								this, _1, _2, _3));

		// Notify that focus finder started
		notifyFocusFinderStarted();

		// First, only simulate a long delay....

		LOG(debug)
		<< "FocusFinderFastCurveLookupT::run - phase 1..." << std::endl;

		// Notify about FoFi update...
		notifyFocusFinderProgressUpdate(0.0, "Starting...");

		// TODO: Set the filter...


		// TODO / HACK....
		for(int i = 0; i < 5; ++i) {

			// TODO: First take picture, then move... -> move down.....?!!!!
			moveFocusByBlocking(FocusDirectionT::INWARD, 1000, 30000ms);

			// This is the focus position for which a star image wil be taken
			int currentFocusPos = getFocus()->getCurrentPos();

			PointT<unsigned int> lastFocusStarPos = getLastFocusStarPos().to<
					unsigned int>();

			LOG(debug)
			<< "FocusFinderFastCurveLookupT::run - focus star pos=("
					<< lastFocusStarPos.x() << ", " << lastFocusStarPos.y() << ")"
					<< std::endl;

			// Calc "extended ROI (we want some more space around the actual star window size -> factor).
			// TODO: Simplify: Overload operator=* -> multiply scalar with size.
			// NOTE: +1 to make the result odd...
			unsigned int factor = 2;
			SizeT<unsigned int> windowSize =
					getFocusFinderProfile().getStarWindowSize();

			// NOTE: In case factor is evenm we need to add +1 to make the result odd.
			int oddMaker = (factor % 2 == 0 ? +1 : 0);

			SizeT<unsigned int> outerWindowSize(
					windowSize.width() * factor + oddMaker,
					windowSize.height() * factor + oddMaker);

			// NOTE: This is in frame coordinates
			auto outerRoi = RectT<unsigned int>::fromCenterPoint(lastFocusStarPos,
					outerWindowSize);

			LOG(debug)
			<< "FocusFinderFastCurveLookupT::run - calculated outer ROI=" << outerRoi
					<< std::endl;

			// Set ROI based on last focus star position
			getCamera()->setRoi(outerRoi);

			// Take a picture
			runExposureBlocking(1000ms /*TODO: Set exp time*/);

			// Calc center of subframe
			PointT<unsigned int> centerOuterSubframe(
					std::ceil(mCurrentImage->width() / 2),
					std::ceil(mCurrentImage->height() / 2));

			LOG(debug)
			<< "FocusFinderFastCurveLookupT::run - centerOuterSubframe="
					<< centerOuterSubframe << std::endl;

			// In outer subframe (mCurrentImage) coordinates
			auto innerRoi = RectT<unsigned int>::fromCenterPoint(
					centerOuterSubframe,
					getFocusFinderProfile().getStarWindowSize());

			LOG(debug)
			<< "FocusFinderFastCurveLookupT::run - calculated inner ROI=" << innerRoi
					<< std::endl;

			// get_crop() from mCurrentImage using innerRoi
			ImageT innerSubFrameImg = mCurrentImage->get_crop(innerRoi.x() /*x0*/,
					innerRoi.y() /*y0*/, innerRoi.x() + innerRoi.width() - 1/*x1*/,
					innerRoi.y() + innerRoi.height() - 1/*y1*/
					);

			LOG(debug)
			<< "FocusFinderFastCurveLookupT::run - innerSubFrameImg size - w="
					<< innerSubFrameImg.width() << ", h="
					<< innerSubFrameImg.height() << std::endl;

			// Determine SNR of recorded area to check if there could be astar around.
			checkIfStarIsThere(innerSubFrameImg);

			// TODO: Do not hardcode IWC, do not hardcode "sub mean"
			ImageT newCenterImage;

			auto newCentroidOpt = CentroidT::calculate(innerSubFrameImg,
					CentroidTypeT::IWC, true /*sub mean*/, &newCenterImage);

			if (!newCentroidOpt) {
				// TODO: Improve logging? ReportingT?
				LOG(error)
				<< "Unable to determine new centroid." << std::endl;

				// TODO: Retry? Or just cancel? For now we cancel...
				throw FocusFinderCancelledExceptionT();
			}

			// NOTE: Those coordinates are in coordinates of the inner subframe! We need them in "outer sub frame" coordinates!
			int deltaX = (outerRoi.width() - innerRoi.width()) / 2;
			int deltaY = (outerRoi.height() - innerRoi.height()) / 2;

			LOG(debug)
			<< "FocusFinderFastCurveLookupT::run - deltaX=" << deltaX << ", deltaY="
					<< deltaY << std::endl;

			PointFT newCentroidInnerRoiCoords = *newCentroidOpt;
			PointFT newCentroidOuterRoiCoords(
					newCentroidInnerRoiCoords.x() + deltaX,
					newCentroidInnerRoiCoords.y() + deltaY);

			LOG(debug)
			<< "FocusFinderFastCurveLookupT::run - new centroid (inner frame)="
					<< newCentroidInnerRoiCoords << std::endl;
			LOG(debug)
			<< "FocusFinderFastCurveLookupT::run - new centroid (outer frame)="
					<< newCentroidOuterRoiCoords << std::endl;

			// Use newly calculated center (newCentroidOuterRoiCoords) to again get sub-frame from mCurrentImage
			// TODO: Currently we just round() the float position to the closest int. This may be
			//       improved by introducing sub-pixel accurracy...
			// TODO: We may introduce a round() function for conversion from PointTF to PointT...?
			PointT<unsigned int> newCentroidRoundedToNextInt(
					std::round(newCentroidOuterRoiCoords.x()),
					std::round(newCentroidOuterRoiCoords.y()));

			LOG(debug)
			<< "FocusFinderFastCurveLookupT::run - new centroid rounded to next int="
					<< newCentroidRoundedToNextInt << std::endl;

			auto innerCorrectedRoi = RectT<unsigned int>::fromCenterPoint(
					newCentroidRoundedToNextInt,
					getFocusFinderProfile().getStarWindowSize());

			LOG(debug)
			<< "FocusFinderFastCurveLookupT::run - calculated inner corrected ROI="
					<< innerCorrectedRoi << std::endl;

			// get_crop() from mCurrentImage using corrected innerRoi
			ImageT innerCorrectedSubFrameImg = mCurrentImage->get_crop(
					innerCorrectedRoi.x() /*x0*/, innerCorrectedRoi.y() /*y0*/,
					innerCorrectedRoi.x() + innerCorrectedRoi.width() - 1/*x1*/,
					innerCorrectedRoi.y() + innerCorrectedRoi.height() - 1/*y1*/
					);

			LOG(debug)
			<< "FocusFinderFastCurveLookupT::run - innerCorrectedSubFrameImgsize - w="
					<< innerCorrectedSubFrameImg.width() << ", h="
					<< innerCorrectedSubFrameImg.height() << std::endl;

			// Determine SNR of recorded area to check if there could be a star around.
			float snr;
			checkIfStarIsThere(innerCorrectedSubFrameImg, &snr);

			// TODO: Calculate HFD
			HfdT hfd(innerCorrectedSubFrameImg);
			LOG(debug)
			<< "FocusFinderFastCurveLookupT::run - HFD: " << hfd.getValue() << std::endl;

			// NOTE / TODO: This only works ifh height() is odd
			size_t centerIdxHorz = std::floor(
					innerCorrectedSubFrameImg.height() / 2);

			FwhmT fwhmHorz(
					ImageSlicerT::slice<SliceDirectionT::HORZ>(
							innerCorrectedSubFrameImg, centerIdxHorz));

			// NOTE / TODO: This only works if width() is odd
			size_t centerIdxVert = std::floor(
					innerCorrectedSubFrameImg.width() / 2);

			FwhmT fwhmVert(
					ImageSlicerT::slice<SliceDirectionT::VERT>(
							innerCorrectedSubFrameImg, centerIdxVert));

			LOG(debug)
			<< "FocusFinderFastCurveLookupT::run - FWHM(HORZ): " << fwhmHorz.getValue()
					<< ", FWHM(VERT): " << fwhmVert.getValue() << std::endl;

			// Convert center coordinates to frame coordinates (absolute position within image)
			PointFT newCentroidAbsRoiCoords(
					newCentroidOuterRoiCoords.x() + outerRoi.x(),
					newCentroidOuterRoiCoords.y() + outerRoi.y());

			// Calculate "drift" (movement of center position since last picture
			std::tuple<float, float> drift(
					newCentroidAbsRoiCoords.x() - lastFocusStarPos.x(),
					newCentroidAbsRoiCoords.y() - lastFocusStarPos.y());

			// Fill all so far collected data into the "FoFi Result Structure"
			auto record = FocusCurveRecordBuilderT()
					.setAbsoluteFocusPos(currentFocusPos)
					.setDrift(drift)
					.setSnr(snr)
					.setHorzFwhm(fwhmHorz)
					.setVertFwhm(fwhmVert)
					.setHfd(hfd)
					.setRoiImage(*mCurrentImage) // Take a copy
					.setCorrectedStarImage(innerCorrectedSubFrameImg)
					.setAbsStarCenterPos(newCentroidAbsRoiCoords)
					.build();

			// Notify about FoFi update...
			notifyFocusFinderProgressUpdate(60.0, "Phase 2 finished.", record);
        }














		using namespace std::chrono_literals;
		std::this_thread::sleep_for(2s);

		// Check if it was cancelled
		checkCancelled();

		// Notify about FoFi update...
		notifyFocusFinderProgressUpdate(30.0, "Phase 1 finished.");

		LOG(debug)
		<< "FocusFinderFastCurveLookupT::run - phase 2..." << std::endl;

		std::this_thread::sleep_for(2s);

		// Check if it was cancelled
		checkCancelled();

		// Notify about FoFi update...
		notifyFocusFinderProgressUpdate(60.0, "Phase 2 finished.");

		LOG(debug)
		<< "FocusFinderFastCurveLookupT::run - phase 3..." << std::endl;

		std::this_thread::sleep_for(2s);

		// Check if it was cancelled
		checkCancelled();

		// HACK!!! TODO: Remove...
		// MyDataContainerT dataPointsToFit;
		// dataPointsToFit.push_back(std::make_pair<float, float>(1, 2));
		// dataPointsToFit.push_back(std::make_pair<float, float>(3, 4));
		// dataPointsToFit.push_back(std::make_pair<float, float>(5, 6));
		// dataPointsToFit.push_back(std::make_pair<float, float>(6, 4));
		// dataPointsToFit.push_back(std::make_pair<float, float>(7, 3));
		// dataPointsToFit.push_back(std::make_pair<float, float>(8, 2));

		// // TODO: See FwhmT for usage....
		// CurveParmsT p = CurveFitAlgorithmT::fitCurve(FittingCurveTypeT::TypeE ct, Rng dps,
		// 	const CurveFitParmsT & curveFitParms,
		// 					     CurveFitSummaryT * outSummary = nullptr);

		
		// TODO: The hyperbolic fit will not be performed here... only after measurement... / at the end...
		// CurveParamsT::TypeT hyperbolCurveParms;
		// HyperbolMatcherT::fitGslLevenbergMarquart<MyDataAccessorT>(
		// 		dataPointsToFit, &hyperbolCurveParms, 0.1f /*EpsAbs*/,
		// 		0.1f /*EpsRel*/);

		// // Print calculated curve parameters
		// LOG(debug)
		// << "a=" << hyperbolCurveParms[CurveParamsT::A_IDX] << std::endl << "b="
		// 		<< hyperbolCurveParms[CurveParamsT::B_IDX] << std::endl << "c="
		// 		<< hyperbolCurveParms[CurveParamsT::C_IDX] << std::endl << "d="
		// 		<< hyperbolCurveParms[CurveParamsT::D_IDX] << std::endl;

		// LOG(debug)
		// << "FocusFinderFastCurveLookupT::run - finished" << std::endl;

		// Notify about FoFi update...
		notifyFocusFinderProgressUpdate(100.0, "Phase 3 finished.");

		// Notify that FoFi was finished...
		notifyFocusFinderFinished();

		focusFinderCleanup();

	} catch (FocusFinderCancelledExceptionT & exc) {
		focusFinderCleanup();

		// TODO: Improve error handling... -> ReportingT?
		LOG(warning)
		<< "Focus finder was cancelled." << std::endl;

		// Notify that FoFi was cancelled...
		notifyFocusFinderCancelled();
	} catch (FocusExceptionT & exc) {
		// TODO: MAybe write one common exception handler for the FoFi here since code repeats!
		focusFinderCleanup();

		// TODO: Improve error handling... -> ReportingT?
		LOG(error)
		<< "Hit focus exception: " << exc.what() << std::endl;

		// Notify that FoFi was cancelled...
		// TODO: Maybe introduce notifyFocusFinderFailed()...
		notifyFocusFinderCancelled();

	} catch (TimeoutExceptionT & exc) {
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

void FocusFinderFastCurveLookupT::focusFinderCleanup() {
	LOG(debug)
	<< "FocusFinderFastCurveLookupT::focusFinderCleanup..." << std::endl;

	getCamera()->unregisterExposureCycleFinishedListener(
			mCameraExposureFinishedConnection);
}

void FocusFinderFastCurveLookupT::cancel() {
	mCancelled = true;
    cv.notify_all();
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
