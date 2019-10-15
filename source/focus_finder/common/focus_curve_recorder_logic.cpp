#include "include/focus_curve_recorder_logic.h"

#include "include/focus_finder_logic.h"
#include "include/point.h"
#include "include/point_with_residual.h"
#include "include/focus_curve_recorder_type.h"
#include "include/focus_curve_recorder_factory.h"
#include "include/focus_curve_recorder.h"
#include "include/profile_manager.h"

class FocusCurveRecordSetT;

const float FocusCurveRecorderLogicT::EPS_REL = 1e-2; // TODO: Do not hardcode
const float FocusCurveRecorderLogicT::EPS_ABS = 1e-2; // TODO: Do not hardcode
const size_t FocusCurveRecorderLogicT::MAX_NUM_ITER = 10000; // TODO: Do not hardcode?
const float FocusCurveRecorderLogicT::OUTLIER_BOUNDARY_FACTOR = 1.5F; // TODO: Do not hardcode?
const float FocusCurveRecorderLogicT::MAX_ACCEPTED_OUTLIERS_PERC = 20.0F; // TODO: Do not hardcode?

FocusCurveRecorderLogicT::FocusCurveRecorderLogicT(FocusFinderLogicT & ffl) : mFfl(ffl), mFocusCurveRecorder(nullptr) {
}

std::shared_ptr<FocusCurveRecorderT> FocusCurveRecorderLogicT::getFocusCurveRecorder() {
  return mFocusCurveRecorder;
}

void FocusCurveRecorderLogicT::resetFocusCurveRecorder(FocusCurveRecorderTypeT::TypeE focusCurveRecorderType) {

    auto focusCurveRecorder = FocusCurveRecorderFactoryT::getInstance(
							FocusCurveRecorderTypeT::DEFAULT);

    // Set devices
    focusCurveRecorder->setCamera(mFfl.getCurrentCamera());
    focusCurveRecorder->setFocus(mFfl.getCurrentFocus());
    focusCurveRecorder->setFilter(mFfl.getCurrentFilter());

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


// TODO: Return FocusCurveT
// TODO: Make public? -> Use from UI?
// TODO: What should be returned? FocusCurveT (shared ptr...?)?
void FocusCurveRecorderLogicT::fitFocusCurve(std::shared_ptr<const FocusCurveRecordSetT> focusCurveRecordSet) {
  CurveParmsT curveParms;
  CurveFitSummaryT curveFitSummary;

  std::vector<PointFT> fitValues;
  std::vector<PointWithResidualT> outlierValues;

  CurveFitParmsT curveFitParms(
			       EPS_REL, /*epsrel*/
			       EPS_ABS, /*epsabs*/
			       MAX_NUM_ITER, /*maxnumiter*/
			       OUTLIER_BOUNDARY_FACTOR, /*outlier boundary factor*/
			       MAX_ACCEPTED_OUTLIERS_PERC /* max. accepted outliers perc. */
			       );

  // TODO: Make selection of Hfd, Fwhm etc possible...
  auto dataPoints = *focusCurveRecordSet | boost::adaptors::transformed([](const auto & r) {
									  float focusMeasure = r->getHfd().getValue();
									  int absFocusPos = r->getCurrentAbsoluteFocusPos();
									  return PointFT(absFocusPos, focusMeasure);
									      });

  
  curveParms = CurveFitAlgorithmT::fitCurve(FittingCurveTypeT::HYPERBOLIC,
					    dataPoints,
					    curveFitParms,
					    & curveFitSummary);
  
  // Transfer over the data to the global storage. Plot the true points onto the graph as well.
  //*outFitValues = curveFitSummary.curveDataPoints; -> only y values? -> fwhm is different han focus curve...
  //boost::range::copy(curveFitSummary.curveDataPoints | boost::adaptors::transformed([](const auto & p){ return p.y(); }), std::back_inserter(*outFitValues));
  
  // Make a copy of values which were matched
  fitValues = curveFitSummary.curveDataPoints;
  
  // Make a copy of outliers
  outlierValues = curveFitSummary.outliers;

  // TODO...
  //  return curveParms;
}
