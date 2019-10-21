#include <chrono>

#include <boost/bind.hpp>

#include "include/default_focus_curve_recorder.h"
#include "include/logging.h"
#include "include/exception.h"
#include "include/wait_for.h"

#include "include/snr.h"
#include "include/centroid.h"
#include "include/hfd.h"
#include "include/fwhm.h"
#include "include/image_slicer.h"
#include "include/focus_measure_type.h"
#include "include/focus_curve_record.h"
#include "include/focus_curve_record_builder.h"
#include "include/focus_curve_record_set.h"
#include "include/curve_fit_algorithm.h"
#include "include/curve_parms.h"
#include "include/point.h"
#include "include/point_with_residual.h"

#include "include/camera.h"
#include "include/focus.h"
#include "include/filter.h"


DefaultFocusCurveRecorderT::DefaultFocusCurveRecorderT() :
  mCancelled(false),
  mIsRunning(false),
  mInitialFocusPos(0),
  mStepSize(1000),
  mFocusMeasureLimit(12.0F) // TODO: Set from outside! Logic should hold it!
{
  LOG(debug)
    << "DefaultFocusCurveRecorderT::DefaultFocusCurveRecorderT..." << std::endl;

  mFocusCurveRecordSets = std::make_shared<FocusCurveRecordSetContainerT>();
}

std::string DefaultFocusCurveRecorderT::getName() const {
	return "DefaultFocusCurveRecorderT";
}

void DefaultFocusCurveRecorderT::reset() {
  
	// TODO: What happens if called while running? -> Exception?
	LOG(debug)
	<< "DefaultFocusCurveRecorderT::reset..." << std::endl;

	mFocusCurveRecordSets->clear();

	mInitialFocusPos = getFocus()->getCurrentPos(); // TODO: Ok? What if no focus set?
}

void DefaultFocusCurveRecorderT::checkCancelled() const {
  if (mCancelled.load()) {
    throw FocusCurveRecorderCancelledExceptionT("Focus curve recorder cancelled.");
  }
}

void DefaultFocusCurveRecorderT::waitForFocus(std::chrono::milliseconds timeout) const {
  auto isFocusPositionReachedOrCancelledLambda = [=, this]() -> bool {
						   LOG(debug) << "current pos=" << getFocus()->getCurrentPos()
							      << ", target pos=" << getFocus()->getTargetPos()
							      << ", isMoving? " << getFocus()->isMoving()
							      << std::endl;

						   // TODO: focus->getTargetPos() currently is not implemented..
						   //       not sure if target and/or current position are maintained by INDI focus driver...
						   //return (focus->getCurrentPos() == focus->getTargetPos() && ! focus->isMoving());
						   return (! getFocus()->isMoving() || mCancelled.load());
						 };
  wait_for(isFocusPositionReachedOrCancelledLambda, timeout);
}

void DefaultFocusCurveRecorderT::moveFocusToBlocking(int absPos, std::chrono::milliseconds timeout) {

  if (getFocus() == nullptr) {
    throw FocusCurveRecorderFailedExceptionT("No focus device set.");
  }
  
  // Set the target position (abs)
  getFocus()->setTargetPos(absPos);

  waitForFocus(timeout);

  // If it was cancelled, throw cancel exception
  if (mCancelled) {
    throw FocusCurveRecorderCancelledExceptionT();
  }
}

void DefaultFocusCurveRecorderT::moveFocusByBlocking(
		FocusDirectionT::TypeE direction, int ticks,
		std::chrono::milliseconds timeout) {

	// Set the target position (rel)
	getFocus()->setTargetPos(ticks, direction);

	waitForFocus(timeout);

	// If it was cancelled, throw cancel exception
	if (mCancelled) {
		throw FocusCurveRecorderCancelledExceptionT();
	}
}

// TODO: Do not just return the one float but all data HfdT, vertical ahnd horizontal FwhmT etc. in a data structure...
std::shared_ptr<FocusCurveRecordT> DefaultFocusCurveRecorderT::measureFocus() {
  using namespace std::chrono_literals;
  
  PointT<unsigned int> lastFocusStarPos = getLastFocusStarPos().to<unsigned int>();

  LOG(debug)
    << "DefaultFocusCurveRecorderT::run - focus star pos=("
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
    << "DefaultFocusCurveRecorderT::run - calculated outer ROI=" << outerRoi
    << std::endl;

  // Set ROI based on last focus star position
  getCamera()->setRoi(outerRoi);
  
  // TODO: Use exposure time set by the user...
  LOG(debug) << "DefaultFocusCurveRecorderT::measureFocus... starting exposure..." << std::endl;

  runExposureBlocking(1000ms);
 
  LOG(debug) << "DefaultFocusCurveRecorderT::measureFocus... exposure finished." << std::endl;


  // Calc center of subframe
  PointT<unsigned int> centerOuterSubframe(
					   std::ceil(mCurrentImage->width() / 2),
					   std::ceil(mCurrentImage->height() / 2));

  LOG(debug)
    << "DefaultFocusCurveRecorderT::run - centerOuterSubframe="
    << centerOuterSubframe << std::endl;

  // In outer subframe (mCurrentImage) coordinates
  auto innerRoi = RectT<unsigned int>::fromCenterPoint(
						       centerOuterSubframe,
						       getFocusFinderProfile().getStarWindowSize());

  LOG(debug)
    << "DefaultFocusCurveRecorderT::run - calculated inner ROI=" << innerRoi
    << std::endl;

  // get_crop() from mCurrentImage using innerRoi
  ImageT innerSubFrameImg = mCurrentImage->get_crop(innerRoi.x() /*x0*/,
						    innerRoi.y() /*y0*/, innerRoi.x() + innerRoi.width() - 1/*x1*/,
						    innerRoi.y() + innerRoi.height() - 1/*y1*/
						    );

  LOG(debug)
    << "DefaultFocusCurveRecorderT::run - innerSubFrameImg size - w="
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
    throw FocusCurveRecorderCancelledExceptionT();
  }

  // NOTE: Those coordinates are in coordinates of the inner subframe! We need them in "outer sub frame" coordinates!
  int deltaX = (outerRoi.width() - innerRoi.width()) / 2;
  int deltaY = (outerRoi.height() - innerRoi.height()) / 2;

  LOG(debug)
    << "DefaultFocusCurveRecorderT::run - deltaX=" << deltaX << ", deltaY="
    << deltaY << std::endl;

  PointFT newCentroidInnerRoiCoords = *newCentroidOpt;
  PointFT newCentroidOuterRoiCoords(
				    newCentroidInnerRoiCoords.x() + deltaX,
				    newCentroidInnerRoiCoords.y() + deltaY);

  LOG(debug)
    << "DefaultFocusCurveRecorderT::run - new centroid (inner frame)="
    << newCentroidInnerRoiCoords << std::endl;
  LOG(debug)
    << "DefaultFocusCurveRecorderT::run - new centroid (outer frame)="
    << newCentroidOuterRoiCoords << std::endl;

  // Use newly calculated center (newCentroidOuterRoiCoords) to again get sub-frame from mCurrentImage
  // TODO: Currently we just round() the float position to the closest int. This may be
  //       improved by introducing sub-pixel accurracy...
  // TODO: We may introduce a round() function for conversion from PointTF to PointT...?
  PointT<unsigned int> newCentroidRoundedToNextInt(
						   std::round(newCentroidOuterRoiCoords.x()),
						   std::round(newCentroidOuterRoiCoords.y()));

  LOG(debug)
    << "DefaultFocusCurveRecorderT::run - new centroid rounded to next int="
    << newCentroidRoundedToNextInt << std::endl;

  auto innerCorrectedRoi = RectT<unsigned int>::fromCenterPoint(
								newCentroidRoundedToNextInt,
								getFocusFinderProfile().getStarWindowSize());

  LOG(debug)
    << "DefaultFocusCurveRecorderT::run - calculated inner corrected ROI="
    << innerCorrectedRoi << std::endl;

  // get_crop() from mCurrentImage using corrected innerRoi
  ImageT innerCorrectedSubFrameImg = mCurrentImage->get_crop(
							     innerCorrectedRoi.x() /*x0*/, innerCorrectedRoi.y() /*y0*/,
							     innerCorrectedRoi.x() + innerCorrectedRoi.width() - 1/*x1*/,
							     innerCorrectedRoi.y() + innerCorrectedRoi.height() - 1/*y1*/
							     );

  LOG(debug)
    << "DefaultFocusCurveRecorderT::run - innerCorrectedSubFrameImgsize - w="
    << innerCorrectedSubFrameImg.width() << ", h="
    << innerCorrectedSubFrameImg.height() << std::endl;

  // Determine SNR of recorded area to check if there could be a star around.
  float snr;
  checkIfStarIsThere(innerCorrectedSubFrameImg, &snr);

  // TODO: Calculate HFD
  HfdT hfd(innerCorrectedSubFrameImg);
  LOG(debug)
    << "DefaultFocusCurveRecorderT::run - HFD: " << hfd.getValue() << std::endl;


  LOG(debug) << "DefaultFocusCurveRecorderT::measureFocus... focus measure calculated: " << hfd.getValue() << std::endl;




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
    << "DefaultFocusCurveRecorderT::run - FWHM(HORZ): " << fwhmHorz.getValue()
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
    .setAbsoluteFocusPos(getFocus()->getCurrentPos())
    .setDrift(drift)
    .setSnr(snr)
    .setHorzFwhm(fwhmHorz)
    .setVertFwhm(fwhmVert)
    .setHfd(hfd)
    .setRoiImage(*mCurrentImage) // Take a copy
    .setCorrectedStarImage(innerCorrectedSubFrameImg)
    .setAbsStarCenterPos(newCentroidAbsRoiCoords)
    .build();

  return record;
}


SelfOrientationResultT DefaultFocusCurveRecorderT::performSelfOrientation() {
  using namespace std::chrono_literals;

  SelfOrientationResultT selfOrientationResult;
  
  selfOrientationResult.record1 = measureFocus();
  //Notify about FocusCurve recorder update...
  //notifyFocusCurveRecorderProgressUpdate(60.0, "Phase 2 finished.", record);
  notifyFocusCurveRecorderNewRecord(selfOrientationResult.record1);

  // TODO: Should this be configurable?
  const float STEP_FACTOR = 3.0F;
  
  moveFocusByBlocking(FocusDirectionT::INWARD, STEP_FACTOR * mStepSize, 30000ms);
    
  selfOrientationResult.record2 = measureFocus();
  //Notify about FocusCurve recorder update...
  //notifyFocusCurveRecorderProgressUpdate(60.0, "Phase 2 finished.", record);
  notifyFocusCurveRecorderNewRecord(selfOrientationResult.record2);

  
  
  
  float focusMeasure1 = selfOrientationResult.record1->getFocusMeasure(getFocusMeasureType());
  float focusMeasure2 = selfOrientationResult.record2->getFocusMeasure(getFocusMeasureType());
  
  // TODO: Should a minimum difference be required?
  bool aboveFocusMeasureLimit = (focusMeasure2 > mFocusMeasureLimit);

  if (focusMeasure2 > focusMeasure1) {
    // We are on the "left" half of the curve
    selfOrientationResult.curveHalf = LeftHalf;
    selfOrientationResult.focusDirectionToLimit = (aboveFocusMeasureLimit ? FocusDirectionT::OUTWARD : FocusDirectionT::INWARD);
  }
  else if (focusMeasure2 < focusMeasure1) {
    // We are on the "right" half of the curve
    selfOrientationResult.curveHalf = RightHalf;
    selfOrientationResult.focusDirectionToLimit = (aboveFocusMeasureLimit ? FocusDirectionT::INWARD : FocusDirectionT::OUTWARD);
  }
  else {
    // No change detected - unable to determine target focus direction - something is probably wrong...
    // Wrong step size? Invalid focus star? Maybe a hotpixel?
    LOG(error)
      << "DefaultFocusCurveRecorderT::determineTargetFocusDirection... Unable to determine target focus direction. No difference in focus measure detected." << std::endl;
    
    // TODO: reporting?
    throw FocusCurveRecorderCancelledExceptionT("Unable to determine target focus direction. No difference in focus measure detected.");
  }
  
  return selfOrientationResult;
}


// TODO: binary search?!
void DefaultFocusCurveRecorderT::moveUntilFocusMeasureLimitReached(const SelfOrientationResultT & selfOrientationResult, float stepSize, float focusMeasureLimit) {
  using namespace std::chrono_literals;

  LOG(debug)
    << "DefaultFocusCurveRecorderT::moveUntilFocusMeasureLimitReached..." << std::endl;

  FocusDirectionT::TypeE limitFocusDirection = selfOrientationResult.focusDirectionToLimit;
  auto curveRecord = selfOrientationResult.record2;
  //Notify about FocusCurve recorder update...
  //notifyFocusCurveRecorderProgressUpdate(60.0, "Phase 2 finished.", record);
  //notifyFocusCurveRecorderProgressUpdate(".", curveRecord);

  bool limitCrossed = false;
  float focusMeasure = curveRecord->getFocusMeasure(getFocusMeasureType());
  bool initiallyBelow = (focusMeasure < focusMeasureLimit);
  
  while(! limitCrossed) {

    LOG(debug)
      << "DefaultFocusCurveRecorderT::moveUntilFocusMeasureLimitReached..."
      << "limit " << focusMeasureLimit
      << " in direction " << FocusDirectionT::asStr(limitFocusDirection)
      << " not yet reached - (focusMeasure=" << focusMeasure << ")"
      << ", focus position=" << getFocus()->getCurrentPos() << "..."
      << ", moving on..." << std::endl;

    moveFocusByBlocking(limitFocusDirection, stepSize, 30000ms);

    curveRecord = measureFocus();
    focusMeasure = curveRecord->getFocusMeasure(getFocusMeasureType());
    
    //Notify about FocusCurve recorder update...
    //notifyFocusCurveRecorderProgressUpdate(60.0, "Phase 2 finished.", record);
    notifyFocusCurveRecorderNewRecord(curveRecord);

    limitCrossed = (initiallyBelow ? focusMeasure >= focusMeasureLimit : focusMeasure <= focusMeasureLimit);
    
    checkCancelled();
  }
  
  LOG(info)
    << "DefaultFocusCurveRecorderT::moveUntilFocusMeasureLimitReached..."
    << "limit " << focusMeasureLimit
    << " in direction " << FocusDirectionT::asStr(limitFocusDirection)
    << " reached - (focusMeasure=" << focusMeasure << ")"
    << ", focus position=" << getFocus()->getCurrentPos() << std::endl;
}


CurveHalfE DefaultFocusCurveRecorderT::locateStartingPosition() {
  LOG(debug)
    << "DefaultFocusCurveRecorderT::locateStartingPosition..." << std::endl;

  SelfOrientationResultT selfOrientationResult = performSelfOrientation();
  
  // Move close to the limiting focus measure (into the determined direction, in steps of size mStepSize)
  moveUntilFocusMeasureLimitReached(selfOrientationResult, mStepSize, mFocusMeasureLimit);

  return selfOrientationResult.curveHalf;
}



std::shared_ptr<FocusCurveRecordSetT> DefaultFocusCurveRecorderT::recordFocusCurveRecordSet(CurveHalfE curveHalf) {
  using namespace std::chrono_literals;
  
  LOG(debug)
    << "DefaultFocusCurveRecorderT::recordFocusCheckpoints..." << std::endl;

  auto focusCurveRecordSet = std::make_shared<FocusCurveRecordSetT>(getFocusMeasureType(), mFocusMeasureLimit);

  // Store recorded focus curve record set
  mFocusCurveRecordSets->push_back(focusCurveRecordSet);

  
  FocusDirectionT::TypeE recordingDirection = (curveHalf == LeftHalf ? FocusDirectionT::OUTWARD : FocusDirectionT::INWARD);
  
  LOG(debug) << "We are on the " << (curveHalf == LeftHalf ? "LEFT" : "RIGHT") << " half... -> recording direction=" << FocusDirectionT::asStr(recordingDirection) << std::endl;

  auto curveRecord = measureFocus();

  focusCurveRecordSet->push_back(curveRecord);
    
    // Notify about FocusCurve recorder update...
    notifyFocusCurveRecorderRecordSetUpdate(focusCurveRecordSet);
  do {
    moveFocusByBlocking(recordingDirection, mStepSize, 30000ms);

    curveRecord = measureFocus();
    
    focusCurveRecordSet->push_back(curveRecord);

    // Notify about FocusCurve recorder update...
    notifyFocusCurveRecorderRecordSetUpdate(focusCurveRecordSet);

    checkCancelled();

    // Just avoid a stop for the first 2 measurements in case the focus measure limit condition may not be true because of noise. 
  } while(focusCurveRecordSet->size() < 3 || curveRecord->getFocusMeasure(getFocusMeasureType()) < mFocusMeasureLimit);

  LOG(debug) << *focusCurveRecordSet << std::endl;

  return focusCurveRecordSet;
}


// NOTE: Callback from camera device, registered in setCamera of base class FocusFinder
void DefaultFocusCurveRecorderT::onImageReceived(RectT<unsigned int> roi,
		std::shared_ptr<const ImageT> image, bool lastFrame) {

  // TODO: Store image.... roi etc...
  mCurrentImage = image;

  cv.notify_all();
}

// TODO: Add binning?
void DefaultFocusCurveRecorderT::runExposureBlocking(
						     std::chrono::milliseconds expTime) {

  // TODO: Check if getCamera() is set...
  
  using namespace std::chrono_literals;

  // Start exposure
  getCamera()->setExposureTime(expTime);
  getCamera()->startExposure();

  // Use condition variable to wait for finished exposure.
  std::unique_lock<std::mutex> lk(cvMutex);
  cv.wait(lk);

  // If it was cancelled, throw cancel exception
  if (mCancelled) {
    throw FocusCurveRecorderCancelledExceptionT();
  }
}

void DefaultFocusCurveRecorderT::checkIfStarIsThere(const ImageT & img,
		float * outSnr) const {
	float snr = SnrT::calculate(img);

	LOG(debug)
	<< "DefaultFocusCurveRecorderT::run - SNR: " << snr << std::endl;

	if (snr < getFocusFinderProfile().getStarDetectionSnrBoundary()) {
		LOG(warning)
		<< "DefaultFocusCurveRecorderT::run - no valid star detected." << std::endl;

		// TODO: How to handle? Just cancel? Or repeat? For now we cancel...
		throw FocusCurveRecorderCancelledExceptionT();
	}

	if (outSnr != nullptr) {
		*outSnr = snr;
	}
}


void DefaultFocusCurveRecorderT::devicesAvailabilityCheck() {
  // Check that camera is there
  if(getCamera() == nullptr) {
    LOG(error) << "No camera set." << std::endl;
    throw FocusCurveRecorderFailedExceptionT("No camera set.");
  }
  
  // Check that focus is there
  if (getFocus() == nullptr) {
    LOG(error) << "No focus set." << std::endl;
    throw FocusCurveRecorderFailedExceptionT("No focus set.");
  }
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
    
    // Register on camera
    // NOTE / TODO: For some reason std::bind did not compile....
    mCameraExposureFinishedConnection =
      getCamera()->registerExposureCycleFinishedListener(
							 boost::bind(&DefaultFocusCurveRecorderT::onImageReceived,
								     this, _1, _2, _3));

  // Notify that focus finder started
  notifyFocusCurveRecorderStarted();

  devicesAvailabilityCheck();

  
  mInitialFocusPos = getFocus()->getCurrentPos();

  CurveHalfE curveHalf = locateStartingPosition();

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
  // 		int currentFocusPos = getFocus()->getCurrentPos();

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
  // 		getCamera()->setRoi(outerRoi);

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
  // 		//       improved by introducing sub-pixel accurracy...
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
  // 				.setRoiImage(*mCurrentImage) // Take a copy
  // 				.setCorrectedStarImage(innerCorrectedSubFrameImg)
  // 				.setAbsStarCenterPos(newCentroidAbsRoiCoords)
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

    
    cleanup();


  } catch (FocusCurveRecorderFailedExceptionT & exc) {
    // TODO: Improve error handling... -> ReportingT?
    LOG(warning)
      << "Focus curve recorder failed." << std::endl;

    // TODO: Problem... cleanup also can take time since focus position may be changed... -> introduce additional state "cancelling"?
    cleanup();
    
    // Notify that FocusCurveRecorder was cancelled...
    // TODO: Introduce notifyFocusCurveRecorderFailed()....
    notifyFocusCurveRecorderCancelled();    
  }
  catch (FocusCurveRecorderCancelledExceptionT & exc) {

    // TODO: Improve error handling... -> ReportingT?
    LOG(warning)
      << "Focus curve recorder was cancelled." << std::endl;

    // TODO: Problem... cleanup also can take time since focus position may be changed... -> introduce additional state "cancelling"?
    cleanup();
    
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
  catch (TimeoutExceptionT & exc) {
  	// TODO: Improve error handling... -> ReportingT?
  	LOG(error)
  	<< "Hit timeout, unable tu finish curve recording." << std::endl;

	cleanup();

  	// Notify that curve recorder was cancelled...
  	// TODO: Maybe introduce notifyFocusFinderFailed()...
  	notifyFocusCurveRecorderCancelled();
  }
  catch(CurveFitExceptionT & exc) {
    // TODO: This should be catched already earlier and should not cancel the entire FocusCurveRecorder
    LOG(warning)
      << "Matching the focus curve failed." << std::endl;
    
    cleanup();
  }
	
  mIsRunning = false;
}

void DefaultFocusCurveRecorderT::cleanup() {
  using namespace std::chrono_literals;

  LOG(debug)
    << "DefaultFocusCurveRecorderT::cleanup..." << std::endl;

  // In any case move the focus to the initial position (if focus device is still available)
  try {
    moveFocusToBlocking(mInitialFocusPos, 30000ms /*timeout*/);
  } catch (FocusCurveRecorderFailedExceptionT & exc) {
    LOG(error) << "DefaultFocusCurveRecorderT::cleanup... no focus device set!" << std::endl;
  }

  getCamera()->unregisterExposureCycleFinishedListener(mCameraExposureFinishedConnection);
}

void DefaultFocusCurveRecorderT::cancel() {
  mCancelled = true;
  cv.notify_all();
}

std::shared_ptr<const FocusCurveRecordSetContainerT> DefaultFocusCurveRecorderT::getFocusCurveRecordSets() const {
  return mFocusCurveRecordSets;
}
