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
#include "include/curve_function_factory.h"

class FocusAnalyzerT;

FocusFinderFastCurveLookupT::FocusFinderFastCurveLookupT(std::shared_ptr<FocusAnalyzerT> focusAnalyzer) : FocusFinderT(focusAnalyzer),
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
		throw FocusAnalyzerCancelledExceptionT("Focus finder cancelled.");
	}
}

bool FocusFinderFastCurveLookupT::isRunning() const {
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
void FocusFinderFastCurveLookupT::run() {

  // TODO TODO TODO: Implement...
  
  // // HACK! For testing only
  // // Hyperbolic curve parms 
  // CurveParmsT parms(4);
  // parms[0] = CurveParmT("IDX_A", 15268.2);
  // parms[1] = CurveParmT("IDX_B", 11.074);
  // parms[2] = CurveParmT("IDX_C", 0); // Was 36347.7 - For the calibration curve, previous abs. focus pos. is not relevant -> 0.
  // parms[3] = CurveParmT("IDX_D", -8.40146); // For slope alone it does not matter, but for a given data point 

  // // Boundaries -> required? -> Better save dx (from center of curve until boundary in both directions, take max.)...
  // float relFocusStepsBoundary = 20590; // Measured (max.)
  // auto focusCurveFunction = CurveFunctionFactoryT::getInstance(FittingCurveTypeT::HYPERBOLIC, parms);



  // std::vector<PointFT> measurements;

  
  // // -> calc from curve! -> Min.
  // float minExpectedFocusMeasure = focusCurveFunction->min().y();

  // // -> calc from curve! (right / outer boundary focus measure value) 
  // float maxExpectedFocusMeasure = focusCurveFunction->f(relFocusStepsBoundary);

  // // Measure focus 1...
  // float absFocusPos1 = 12315; // TODO...
  // float focusMeasure1 = 9.3; // TODO...

  // measurements.push_back(PointFT(absFocusPos1, focusMeasure1));

  
  // // So far, there are two possible locations on the curve..., and we don't know which one to choose... (more precisely, we have +/-x) -> we need a second point...
  // std::vector<float> possibleFocusPositions = focusCurveFunction->f_inv(focusMeasure1);

  
  // // Move focus -> which direction? -> use minExpectedFocusMeasure, maxExpectedFocusMeasure and focusMeasure1 to decide? -> does not make sense... can be on both positions of the curve! Randfälle testen: z.B. kann man nicht nach links gehen, wenn man schon am linken Rand ist, und umgekehrt - bzw. der Abstand zum jeweiligen Rand kleiner als "faktor * stepSize" ist.
  // // Moving by "factor * stepSize" may be problematic when almost in focus...
  // // maybe this code and the code below can be removed and the code from the focus curve recorder can be used!
  // move(factor * stepSize);

  // // Assume, we moved outwards (to the right...) and measure:
  // // Measure focus 2...
  // float absFocusPos2 = 16315; // TODO...
  // float focusMeasure2 = 7.1; // TODO...

  // measurements.push_back(PointFT(absFocusPos2, focusMeasure2));

  
  // // TODO: This logic can probably be written much shorter!
  // // TODO: See curve recorder logic.... -> same problem... same code, and better wording...
  // CurveHalfE curveHalf = determineCurveSide() {
  //   if (direction == OUTWARDS) {
  //     if (focusImproved) { // focusMeasure1 > focusMeasure2
  // 	// -> left part of the curve
  //     }
  //     else if(! focusWorsened) { // focusMeasure1 < focusMeasure2
  // 	// -> right part of the curve      
  //     } else {
  // 	// Problem, focus did not move?
  //     }
  //   } else {
  //     if (focusImproved) { // focusMeasure1 > focusMeasure2
  // 	// -> right part of the curve
  //     }
  //     else if(! focusWorsened) { // focusMeasure1 < focusMeasure2
  // 	// -> left part of the curve      
  //     } else {
  // 	// Problem, focus did not move?
  //     }
  //   }
  // }

  // //std::vector<float> possibleFocusPositions2 = focusCurveFunction->f_inv(focusMeasure2);
  

  // float estimateRelPos(focusCurveFunction, curveHalf) {
  //     std::vector<float> possibleFocusPositions = focusCurveFunction->f_inv(focusMeasure1);
  //     if (curveHalf == ...) {
  // 	return ...;
  //     } else if () {
  // 	return ...;
  //     }
  // }


  // int distBetweenMeasurements = stepSize; // NOTE: Here only use stepSize, factor * stepSize is only used for initial step to determine direction.
  // int distMovedSoFar = ; // CALC, since we already moved! -> move on top
  // float sum = 0;
  // size_t numMeasurements = 0;
  // float stillToMove = 0;



  
  // float averagedDistanceEstimationToBestFocus = calcAverageRealInitialDist(measurements);
  // float distLeftToBestPos = averagedDistanceEstimationToBestFocus - distMovedSoFar;

 
  // while(distLeftToBestPos > distanceBetweenMeasurements) {

  //   move(distBetweenMeasurements); // Direction depends on curveHalf... / prev. movement direction...
  //   distMovedSoFar += distBetweenMeasurements;

  //   tie(absFocusPos, focusMeasure) = measure();
    
  //   measurements.push_back(PointFT(absFocusPos, focusMeasure));

    
  //   averagedDistanceEstimationToBestFocus = calcAverageRealInitialDist(measurements);

  //   distLeftToBestPos = averagedDistanceEstimationToBestFocus - distMovedSoFar;
  // }
  
  // // Finally, move and measure one last time 
  // move(distLeftToBestPos);
  // measure();




  

  // float calcAverageRealInitialDist(const std::vector<PointFT> & measurements, focusCurveFunction, curveHalf) {
  //   float sum = 0;

  //   // TODO: measurements should be sorted by absFocusPos..., starting with smallest (?) Or does this depend on the curveHalf?

    
  //   // We have at least two points in here...
  //   // We iterate so that we analyze measurements 0 & 1, 1 & 2, 2 & 3, etc.
  //   int sum = 0;

  //   PointFT p0 = measurements.at(0);
  //   float focusMeasure = p0.y();
  //   float estimatedRelFocusPos = estimateRelPos(focusCurveFunction, curveHalf, focusMeasure);

  //   sum += estimatedRelFocusPos;
    
    
  //   for (size_t idx = 0; idx < measurements.size() - 1; ++idx) {
  //     PointFT pi = measurements.at(idx);
  //     PointFT pj = measurements.at(idx + 1);

  //     int deltaSteps = std::abs(p0.x() - p1.x());

  //     sum += deltaSteps;
      
  //   }

    
  //   estimatedPos = estimateRelPos(focusCurveFunction, curveHalf);

    
  //   sum += distMovedSoFar + std::abs(estimateRelPos);

    
  //   return sum / (float) measurements.size();
  // }
  
  // // TODO: 

  
  
  // // int lowerAbsFocusPosBoundary = absFocusPos1 - relFocusStepsBoundary;
  // // int lpperAbsFocusPosBoundary = absFocusPos1 + relFocusStepsBoundary;





  
    
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
		// TODO: Remove... moved to focus_analyzer.cpp constructor
		// mCameraExposureFinishedConnection =
		// 		getCamera()->registerExposureCycleFinishedListener(
		// 				boost::bind(&FocusFinderFastCurveLookupT::onImageReceived,
		// 						this, _1, _2, _3));

		// Notify that focus finder started
		notifyFocusFinderStarted();

		// First, only simulate a long delay....

		LOG(debug)
		<< "FocusFinderFastCurveLookupT::run - phase 1..." << std::endl;

		// Notify about FoFi update...
		notifyFocusFinderProgressUpdate(0.0, "Starting...");

		// TODO: Set the filter...


	// 	// TODO / HACK....
		// TODO: Use function measure() which was moved to FocusAnalyzerT...
	// 	for(int i = 0; i < 5; ++i) {

	// 		// TODO: First take picture, then move... -> move down.....?!!!!
	// 		getFocusAnalyzer()->moveFocusByBlocking(FocusDirectionT::INWARD, 1000, 30000ms);

	// 		// This is the focus position for which a star image wil be taken
	// 		int currentFocusPos = getFocusAnalyzer()->getFocus()->getCurrentPos();

	// 		PointT<unsigned int> lastFocusStarPos = getFocusAnalyzer()->getLastFocusStarPos().to<
	// 				unsigned int>();

	// 		LOG(debug)
	// 		<< "FocusFinderFastCurveLookupT::run - focus star pos=("
	// 				<< lastFocusStarPos.x() << ", " << lastFocusStarPos.y() << ")"
	// 				<< std::endl;

	// 		// Calc "extended ROI (we want some more space around the actual star window size -> factor).
	// 		// TODO: Simplify: Overload operator=* -> multiply scalar with size.
	// 		// NOTE: +1 to make the result odd...
	// 		unsigned int factor = 2;
	// 		SizeT<unsigned int> windowSize =
	// 				getFocusAnalyzer()->getFocusFinderProfile().getStarWindowSize();

	// 		// NOTE: In case factor is evenm we need to add +1 to make the result odd.
	// 		int oddMaker = (factor % 2 == 0 ? +1 : 0);

	// 		SizeT<unsigned int> outerWindowSize(
	// 				windowSize.width() * factor + oddMaker,
	// 				windowSize.height() * factor + oddMaker);

	// 		// NOTE: This is in frame coordinates
	// 		auto outerRoi = RectT<unsigned int>::fromCenterPoint(lastFocusStarPos,
	// 				outerWindowSize);

	// 		LOG(debug)
	// 		<< "FocusFinderFastCurveLookupT::run - calculated outer ROI=" << outerRoi
	// 				<< std::endl;

	// 		// Set ROI based on last focus star position
	// 		getFocusAnalyzer()->getCamera()->setRoi(outerRoi);

	// 		// Take a picture
	// 		runExposureBlocking(1000ms /*TODO: Set exp time*/);

	// 		// Calc center of subframe
	// 		PointT<unsigned int> centerOuterSubframe(
	// 				std::ceil(mCurrentImage->width() / 2),
	// 				std::ceil(mCurrentImage->height() / 2));

	// 		LOG(debug)
	// 		<< "FocusFinderFastCurveLookupT::run - centerOuterSubframe="
	// 				<< centerOuterSubframe << std::endl;

	// 		// In outer subframe (mCurrentImage) coordinates
	// 		auto innerRoi = RectT<unsigned int>::fromCenterPoint(
	// 				centerOuterSubframe,
	// 				getFocusFinderProfile().getStarWindowSize());

	// 		LOG(debug)
	// 		<< "FocusFinderFastCurveLookupT::run - calculated inner ROI=" << innerRoi
	// 				<< std::endl;

	// 		// get_crop() from mCurrentImage using innerRoi
	// 		ImageT innerSubFrameImg = mCurrentImage->get_crop(innerRoi.x() /*x0*/,
	// 				innerRoi.y() /*y0*/, innerRoi.x() + innerRoi.width() - 1/*x1*/,
	// 				innerRoi.y() + innerRoi.height() - 1/*y1*/
	// 				);

	// 		LOG(debug)
	// 		<< "FocusFinderFastCurveLookupT::run - innerSubFrameImg size - w="
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
	// 		<< "FocusFinderFastCurveLookupT::run - deltaX=" << deltaX << ", deltaY="
	// 				<< deltaY << std::endl;

	// 		PointFT newCentroidInnerRoiCoords = *newCentroidOpt;
	// 		PointFT newCentroidOuterRoiCoords(
	// 				newCentroidInnerRoiCoords.x() + deltaX,
	// 				newCentroidInnerRoiCoords.y() + deltaY);

	// 		LOG(debug)
	// 		<< "FocusFinderFastCurveLookupT::run - new centroid (inner frame)="
	// 				<< newCentroidInnerRoiCoords << std::endl;
	// 		LOG(debug)
	// 		<< "FocusFinderFastCurveLookupT::run - new centroid (outer frame)="
	// 				<< newCentroidOuterRoiCoords << std::endl;

	// 		// Use newly calculated center (newCentroidOuterRoiCoords) to again get sub-frame from mCurrentImage
	// 		// TODO: Currently we just round() the float position to the closest int. This may be
	// 		//       improved by introducing sub-pixel accurracy...
	// 		// TODO: We may introduce a round() function for conversion from PointTF to PointT...?
	// 		PointT<unsigned int> newCentroidRoundedToNextInt(
	// 				std::round(newCentroidOuterRoiCoords.x()),
	// 				std::round(newCentroidOuterRoiCoords.y()));

	// 		LOG(debug)
	// 		<< "FocusFinderFastCurveLookupT::run - new centroid rounded to next int="
	// 				<< newCentroidRoundedToNextInt << std::endl;

	// 		auto innerCorrectedRoi = RectT<unsigned int>::fromCenterPoint(
	// 				newCentroidRoundedToNextInt,
	// 				getFocusFinderProfile().getStarWindowSize());

	// 		LOG(debug)
	// 		<< "FocusFinderFastCurveLookupT::run - calculated inner corrected ROI="
	// 				<< innerCorrectedRoi << std::endl;

	// 		// get_crop() from mCurrentImage using corrected innerRoi
	// 		ImageT innerCorrectedSubFrameImg = mCurrentImage->get_crop(
	// 				innerCorrectedRoi.x() /*x0*/, innerCorrectedRoi.y() /*y0*/,
	// 				innerCorrectedRoi.x() + innerCorrectedRoi.width() - 1/*x1*/,
	// 				innerCorrectedRoi.y() + innerCorrectedRoi.height() - 1/*y1*/
	// 				);

	// 		LOG(debug)
	// 		<< "FocusFinderFastCurveLookupT::run - innerCorrectedSubFrameImgsize - w="
	// 				<< innerCorrectedSubFrameImg.width() << ", h="
	// 				<< innerCorrectedSubFrameImg.height() << std::endl;

	// 		// Determine SNR of recorded area to check if there could be a star around.
	// 		float snr;
	// 		checkIfStarIsThere(innerCorrectedSubFrameImg, &snr);

	// 		// TODO: Calculate HFD
	// 		HfdT hfd(innerCorrectedSubFrameImg);
	// 		LOG(debug)
	// 		<< "FocusFinderFastCurveLookupT::run - HFD: " << hfd.getValue() << std::endl;

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
	// 		<< "FocusFinderFastCurveLookupT::run - FWHM(HORZ): " << fwhmHorz.getValue()
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
	// 		auto record = FocusCurveRecordBuilderT()
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

	} catch (FocusAnalyzerCancelledExceptionT & exc) {
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
}

void FocusFinderFastCurveLookupT::cancel() {
	mCancelled = true;
	getFocusAnalyzer()->cancel();
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
