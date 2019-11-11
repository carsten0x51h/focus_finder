// TODO: Remove the includes which are not required...
#include <chrono>
//#include <thread>
//#include <condition_variable>
#include <mutex>

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
#include "include/curve_half.h"
#include "include/point.h"
#include "include/point_with_residual.h"
#include "include/self_orientation_result.h"

#include "include/camera.h"
#include "include/focus.h"
#include "include/filter.h"

#include "include/focus_analyzer.h"


//FocusAnalyzerT::FocusAnalyzerT() : mCamera(nullptr), mFocus(nullptr), mFilter(nullptr) {
FocusAnalyzerT::FocusAnalyzerT(std::shared_ptr<CameraT> camera, std::shared_ptr<FocusT> focus, std::shared_ptr<FilterT> filter) : mCancelled(false), mCamera(camera), mFocus(focus), mFilter(filter) {

  // TODO: Check if require devices are != nullptr?! Or pass by reference?
  
  // TODO / FIXME: This won't work if the profile changes and the current instance of the FocusAnalyzer would stay alife. Then it would be registered to the old camera...


    mCameraExposureFinishedConnection =
      getCamera()->registerExposureCycleFinishedListener(
							 boost::bind(&FocusAnalyzerT::onImageReceived,
								     this, _1, _2, _3));

  mInitialFocusPos = getFocus()->getCurrentPos();
}

FocusAnalyzerT::~FocusAnalyzerT()
{
  cleanup();
}


std::shared_ptr<CameraT> FocusAnalyzerT::getCamera() const
{
  return mCamera;
}
// void FocusAnalyzerT::setCamera(std::shared_ptr<CameraT> camera)
// {
//   mCamera = camera;
// }

std::shared_ptr<FocusT> FocusAnalyzerT::getFocus() const
{
  return mFocus;
}
// void FocusAnalyzerT::setFocus(std::shared_ptr<FocusT> focus)
// {
//   mFocus = focus;
// }

std::shared_ptr<FilterT> FocusAnalyzerT::getFilter() const
{
  return mFilter;
}
// void FocusAnalyzerT::setFilter(std::shared_ptr<FilterT> filter)
// {
//   mFilter = filter;
// }

// TODO: Maybe we find a better name?!
void FocusAnalyzerT::setLastFocusStarPos(PointT<float> lastFocusStarPos)
{
  mLastFocusStarPos = lastFocusStarPos;
}
PointT<float> FocusAnalyzerT::getLastFocusStarPos() const
{
  return mLastFocusStarPos;
}

void FocusAnalyzerT::setFocusFinderProfile(const FocusFinderProfileT & profile)
{
  mFocusFinderProfile = profile;
}
const FocusFinderProfileT & FocusAnalyzerT::getFocusFinderProfile() const
{
  return mFocusFinderProfile;
}


// TODO: Do not just return the one float but all data HfdT, vertical ahnd horizontal FwhmT etc. in a data structure...
std::shared_ptr<FocusCurveRecordT> FocusAnalyzerT::measureFocus() {
  using namespace std::chrono_literals;
  
  std::shared_ptr<FocusCurveRecordT> record = nullptr;  
  PointT<unsigned int> lastFocusStarPos = getLastFocusStarPos().to<unsigned int>();

  LOG(debug)
    << "FocusAnalyzerT::run - focus star pos=("
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
    << "FocusAnalyzerT::run - calculated outer ROI=" << outerRoi
    << std::endl;

  // Set ROI based on last focus star position
  getCamera()->setRoi(outerRoi);


  const int retries = 3;
  int retry = 0;
  bool success = false;
  
  while(! success && retry < retries) {
    try {
  
      // TODO: Use exposure time set by the user...
      LOG(debug) << "FocusAnalyzerT::measureFocus... starting exposure..." << std::endl;

      runExposureBlocking(1000ms);
 
      LOG(debug) << "FocusAnalyzerT::measureFocus... exposure finished." << std::endl;


      // Calc center of subframe
      PointT<unsigned int> centerOuterSubframe(
					       std::ceil(mCurrentImage->width() / 2),
					       std::ceil(mCurrentImage->height() / 2));

      LOG(debug)
	<< "FocusAnalyzerT::run - centerOuterSubframe="
	<< centerOuterSubframe << std::endl;

      // In outer subframe (mCurrentImage) coordinates
      auto innerRoi = RectT<unsigned int>::fromCenterPoint(
							   centerOuterSubframe,
							   getFocusFinderProfile().getStarWindowSize());

      LOG(debug)
	<< "FocusAnalyzerT::run - calculated inner ROI=" << innerRoi
	<< std::endl;

      // get_crop() from mCurrentImage using innerRoi
      ImageT innerSubFrameImg = mCurrentImage->get_crop(innerRoi.x() /*x0*/,
							innerRoi.y() /*y0*/, innerRoi.x() + innerRoi.width() - 1/*x1*/,
							innerRoi.y() + innerRoi.height() - 1/*y1*/
							);

      LOG(debug)
	<< "FocusAnalyzerT::run - innerSubFrameImg size - w="
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
	throw FocusAnalyzerCancelledExceptionT();
      }

      // NOTE: Those coordinates are in coordinates of the inner subframe! We need them in "outer sub frame" coordinates!
      int deltaX = (outerRoi.width() - innerRoi.width()) / 2;
      int deltaY = (outerRoi.height() - innerRoi.height()) / 2;

      LOG(debug)
	<< "FocusAnalyzerT::run - deltaX=" << deltaX << ", deltaY="
	<< deltaY << std::endl;

      PointFT newCentroidInnerRoiCoords = *newCentroidOpt;
      PointFT newCentroidOuterRoiCoords(
					newCentroidInnerRoiCoords.x() + deltaX,
					newCentroidInnerRoiCoords.y() + deltaY);

      LOG(debug)
	<< "FocusAnalyzerT::run - new centroid (inner frame)="
	<< newCentroidInnerRoiCoords << std::endl;
      LOG(debug)
	<< "FocusAnalyzerT::run - new centroid (outer frame)="
	<< newCentroidOuterRoiCoords << std::endl;

      // Use newly calculated center (newCentroidOuterRoiCoords) to again get sub-frame from mCurrentImage
      // TODO: Currently we just round() the float position to the closest int. This may be
      //       improved by introducing sub-pixel accurracy...
      // TODO: We may introduce a round() function for conversion from PointTF to PointT...?
      PointT<unsigned int> newCentroidRoundedToNextInt(
						       std::round(newCentroidOuterRoiCoords.x()),
						       std::round(newCentroidOuterRoiCoords.y()));

      LOG(debug)
	<< "FocusAnalyzerT::run - new centroid rounded to next int="
	<< newCentroidRoundedToNextInt << std::endl;

      auto innerCorrectedRoi = RectT<unsigned int>::fromCenterPoint(
								    newCentroidRoundedToNextInt,
								    getFocusFinderProfile().getStarWindowSize());

      LOG(debug)
	<< "FocusAnalyzerT::run - calculated inner corrected ROI="
	<< innerCorrectedRoi << std::endl;

      // get_crop() from mCurrentImage using corrected innerRoi
      ImageT innerCorrectedSubFrameImg = mCurrentImage->get_crop(
								 innerCorrectedRoi.x() /*x0*/, innerCorrectedRoi.y() /*y0*/,
								 innerCorrectedRoi.x() + innerCorrectedRoi.width() - 1/*x1*/,
								 innerCorrectedRoi.y() + innerCorrectedRoi.height() - 1/*y1*/
								 );

      LOG(debug)
	<< "FocusAnalyzerT::run - innerCorrectedSubFrameImgsize - w="
	<< innerCorrectedSubFrameImg.width() << ", h="
	<< innerCorrectedSubFrameImg.height() << std::endl;

      // Determine SNR of recorded area to check if there could be a star around.
      float snr;
      checkIfStarIsThere(innerCorrectedSubFrameImg, &snr);

      // TODO: Calculate HFD
      HfdT hfd(innerCorrectedSubFrameImg);
      LOG(debug)
	<< "FocusAnalyzerT::run - HFD: " << hfd.getValue() << std::endl;


      LOG(debug) << "FocusAnalyzerT::measureFocus... focus measure calculated: " << hfd.getValue() << std::endl;




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
	<< "FocusAnalyzerT::run - FWHM(HORZ): " << fwhmHorz.getValue()
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
      record = FocusCurveRecordBuilderT()
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

      success = true;

      LOG(info) << "Focus measure successfully finished." << std::endl;
      
    } catch(FwhmExceptionT & exc) {
      LOG(warning) << "Unable to calculate FWHM... re-taking image... (retrying " << retry << "/" << retries << ")..." << std::endl;
      success = false;
    }
  } // end while

  if (! success) {
    // TODO: THROW EXC - not possible to match after N retries
    LOG(error) << "Unable to calculate FWHM after " << retries << " retries." << std::endl;
  }
  
  return record;
}


void FocusAnalyzerT::waitForFocus(std::chrono::milliseconds timeout) const {
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





// TODO: binary search?!
// BOUNDARY SCAN!
void FocusAnalyzerT::moveUntilFocusMeasureLimitReached(const SelfOrientationResultT & selfOrientationResult, float stepSize, float focusMeasureLimit) {
  using namespace std::chrono_literals;

  LOG(debug)
    << "FocusAnalyzerT::moveUntilFocusMeasureLimitReached..." << std::endl;


  // TODO: Get values from focus device
  int minAbsFocusPos = 0;
  int maxAbsFocusPos = 80000;


  
  
  FocusDirectionT::TypeE limitFocusDirection = selfOrientationResult.focusDirectionToLimit;
  auto curveRecord = selfOrientationResult.record2;

  //Notify about FocusCurve recorder update...

  notifyFocusAnalyzerProgressUpdate(0.0F, "Boundary scan running...", curveRecord);
  //notifyFocusCurveRecorderProgressUpdate(".", curveRecord);

  int stepNo = 0;
  bool limitCrossed = false;
  float focusMeasure = curveRecord->getFocusMeasure(mFocusFinderProfile.getLimitFocusMeasureType());
  bool initiallyBelow = (focusMeasure < focusMeasureLimit);
  
  while(! limitCrossed) {

    LOG(debug)
      << "FocusAnalyzerT::moveUntilFocusMeasureLimitReached..."
      << "limit " << focusMeasureLimit
      << " in direction " << FocusDirectionT::asStr(limitFocusDirection)
      << " not yet reached - (focusMeasure=" << focusMeasure << ")"
      << ", focus position=" << getFocus()->getCurrentPos() << "..."
      << ", moving on..." << std::endl;
    
    moveFocusByBlocking(limitFocusDirection, stepSize, 30000ms);

    curveRecord = measureFocus();
    focusMeasure = curveRecord->getFocusMeasure(mFocusFinderProfile.getLimitFocusMeasureType());
    
    //Notify about FocusCurve recorder update...
    //notifyFocusCurveRecorderProgressUpdate(60.0, "Phase 2 finished.", record);

    float maxPossibleStepsIntoLimitDirection = (limitFocusDirection == FocusDirectionT::INWARD ? (getFocus()->getCurrentPos() - minAbsFocusPos) / (float) stepSize : (maxAbsFocusPos - getFocus()->getCurrentPos()) / (float) stepSize);
    float progressPerc = 100.0F * (stepNo / maxPossibleStepsIntoLimitDirection);

    LOG(warning) << "pos: " << getFocus()->getCurrentPos() << ", maxPossibleStepsIntoLimitDirection: " << maxPossibleStepsIntoLimitDirection << ", progressPerc: " << progressPerc << std::endl;
    
  
    notifyFocusAnalyzerProgressUpdate(progressPerc, "Boundary scan running...", curveRecord);

    notifyFocusAnalyzerNewRecord(curveRecord);

    limitCrossed = (initiallyBelow ? focusMeasure >= focusMeasureLimit : focusMeasure <= focusMeasureLimit);
    
    checkCancelled();

    ++stepNo;
  }

  notifyFocusAnalyzerProgressUpdate(100.0F, "Boundary scan finished.");

  LOG(info)
    << "FocusAnalyzerT::moveUntilFocusMeasureLimitReached..."
    << "limit " << focusMeasureLimit
    << " in direction " << FocusDirectionT::asStr(limitFocusDirection)
    << " reached - (focusMeasure=" << focusMeasure << ")"
    << ", focus position=" << getFocus()->getCurrentPos() << std::endl;
}


void FocusAnalyzerT::checkCancelled() const {
  if (mCancelled.load()) {
    throw FocusAnalyzerCancelledExceptionT("Focus curve recorder cancelled.");
  }
}

void FocusAnalyzerT::cancel() {
  mCancelled = true;
  cv.notify_all();
}

// NOTE: Callback from camera device, registered in setCamera of base class FocusFinder
void FocusAnalyzerT::onImageReceived(RectT<unsigned int> roi,
		std::shared_ptr<const ImageT> image, bool lastFrame) {

  // TODO: Store image.... roi etc...
  mCurrentImage = image;

  cv.notify_all();
}


SelfOrientationResultT FocusAnalyzerT::performSelfOrientation() {
  using namespace std::chrono_literals;

  SelfOrientationResultT selfOrientationResult;
  
  selfOrientationResult.record1 = measureFocus();
  //Notify about FocusCurve recorder update...
  notifyFocusAnalyzerProgressUpdate(0.0f, "Self orientation running....", selfOrientationResult.record1);
  notifyFocusAnalyzerNewRecord(selfOrientationResult.record1);

  // TODO: Should this be configurable?
  const float STEP_FACTOR = 3.0F;
  
  moveFocusByBlocking(FocusDirectionT::INWARD, STEP_FACTOR * mFocusFinderProfile.getStepSize(), 30000ms);
    
  selfOrientationResult.record2 = measureFocus();
  //Notify about FocusCurve recorder update...
  notifyFocusAnalyzerProgressUpdate(100.0F, "Self orientation finished.", selfOrientationResult.record2);
  notifyFocusAnalyzerNewRecord(selfOrientationResult.record2);

  FocusMeasureTypeT::TypeE curveFocusMeasureType = mFocusFinderProfile.getCurveFocusMeasureType();
  
  float focusMeasure1 = selfOrientationResult.record1->getFocusMeasure(curveFocusMeasureType);
  float focusMeasure2 = selfOrientationResult.record2->getFocusMeasure(curveFocusMeasureType);
  
  // TODO: Should a minimum difference be required?
  // TODO / FIXME!! mFocusMeasureLimit no longer defined... the limiting value depends on the FocusMeasureType! So either the appl. has some good defaults ( can calculate them depending on camera and telescope settings etc., or the user can specify default values for all the focus measures in the profile. Furthermore, this value could simply be supplied as a parameter to this function! Then the selfOrientation function does not care if the value comes from the profile, a pre-calculation or a UI....
  bool aboveFocusMeasureLimit = (focusMeasure2 > 12.0F/*FIXME!!mFocusMeasureLimit*/);

  if (focusMeasure2 > focusMeasure1) {
    // We are on the "left" half of the curve
    selfOrientationResult.curveHalf = CurveHalfT::LEFT_HALF;
    selfOrientationResult.focusDirectionToLimit = (aboveFocusMeasureLimit ? FocusDirectionT::OUTWARD : FocusDirectionT::INWARD);
  }
  else if (focusMeasure2 < focusMeasure1) {
    // We are on the "right" half of the curve
    selfOrientationResult.curveHalf = CurveHalfT::RIGHT_HALF;
    selfOrientationResult.focusDirectionToLimit = (aboveFocusMeasureLimit ? FocusDirectionT::INWARD : FocusDirectionT::OUTWARD);
  }
  else {
    // No change detected - unable to determine target focus direction - something is probably wrong...
    // Wrong step size? Invalid focus star? Maybe a hotpixel?
    LOG(error)
      << "FocusAnalyzerT::determineTargetFocusDirection... Unable to determine target focus direction. No difference in focus measure detected." << std::endl;
    
    // TODO: reporting?
    throw FocusAnalyzerCancelledExceptionT("Unable to determine target focus direction. No difference in focus measure detected.");
  }
  
  return selfOrientationResult;
}


void FocusAnalyzerT::moveFocusByBlocking(
		FocusDirectionT::TypeE direction, int ticks,
		std::chrono::milliseconds timeout) {

	// Set the target position (rel)
	getFocus()->setTargetPos(ticks, direction);

	waitForFocus(timeout);

	// If it was cancelled, throw cancel exception
	if (mCancelled) {
		throw FocusAnalyzerCancelledExceptionT();
	}
}


void FocusAnalyzerT::moveFocusToBlocking(int absPos, std::chrono::milliseconds timeout) {

  if (getFocus() == nullptr) {
    throw FocusAnalyzerFailedExceptionT("No focus device set.");
  }
  
  // Set the target position (abs)
  getFocus()->setTargetPos(absPos);

  waitForFocus(timeout);

  // If it was cancelled, throw cancel exception
  if (mCancelled) {
    throw FocusAnalyzerCancelledExceptionT("Unable to determine target focus direction. No difference in focus measure detected.");
  }
}


// TODO: Add binning?
void FocusAnalyzerT::runExposureBlocking(
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
    throw FocusAnalyzerCancelledExceptionT();
  }
}


void FocusAnalyzerT::checkIfStarIsThere(const ImageT & img,
		float * outSnr) const {
	float snr = SnrT::calculate(img);

	LOG(debug)
	<< "FocusAnalyzerT::run - SNR: " << snr << std::endl;

	if (snr < getFocusFinderProfile().getStarDetectionSnrBoundary()) {
		LOG(warning)
		<< "FocusAnalyzerT::run - no valid star detected." << std::endl;

		// TODO: How to handle? Just cancel? Or repeat? For now we cancel...
		throw FocusAnalyzerCancelledExceptionT();
	}

	if (outSnr != nullptr) {
		*outSnr = snr;
	}
}


void FocusAnalyzerT::devicesAvailabilityCheck() {
  // Check that camera is there
  if(getCamera() == nullptr) {
    LOG(error) << "No camera set." << std::endl;
    // TODO: Rename exception!
    throw FocusAnalyzerFailedExceptionT("No camera set.");
  }
  
  // Check that focus is there
  if (getFocus() == nullptr) {
    LOG(error) << "No focus set." << std::endl;
    throw FocusAnalyzerFailedExceptionT("No focus set.");
  }
}

void FocusAnalyzerT::cleanup() {
  using namespace std::chrono_literals;

  LOG(debug)
    << "FocusAnalyzerT::cleanup..." << std::endl;

  // In any case move the focus to the initial position (if focus device is still available)
  try {
    moveFocusToBlocking(mInitialFocusPos, 30000ms /*timeout*/);
  } catch (FocusAnalyzerFailedExceptionT & exc) {
    LOG(error) << "FocusAnalyzerT::cleanup... no focus device set!" << std::endl;
  }

  // NOTE: Re-register?!?! -> no... cleanup() should only be called at the end-of-life of the FocusAnalyzer object.
  // TODO: Check if getCamera() still returns valid device...
  getCamera()->unregisterExposureCycleFinishedListener(mCameraExposureFinishedConnection);
}

