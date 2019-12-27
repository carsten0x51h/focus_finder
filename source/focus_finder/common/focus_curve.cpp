#include "include/focus_curve.h"
#include "include/focus_curve_record_set.h"
#include "include/curve_function_factory.h"

// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/ini_parser.hpp>

// FocusCurveT::FocusCurveT(FittingCurveTypeT::TypeE curveType, const CurveParmsT & curveParms, FocusMeasureTypeT::TypeE focusMeasureType, int lowerFocusPos, int upperFocusPos, float focusMeasureLimit) : mFocusCurveType(curveType), mCurveParms(curveParms), mFocusMeasureType(focusMeasureType), mLowerFocusPos(lowerFocusPos), mUpperFocusPos(upperFocusPos), mFocusMeasureLimit(focusMeasureLimit) {
// }


FocusCurveT::FocusCurveT(std::shared_ptr<const FocusCurveRecordSetT> focusCurveRecordSet, const CurveFitParmsT & curveFitParms) {

  mFocusMeasureType = focusCurveRecordSet->getFocusMeasureType(); // TODO: Good idea to store the focus measure type in the RecordSet?
  mFocusMeasureLimit = focusCurveRecordSet->getFocusMeasureLimit(); // TODO: Good idea to store this in the RecordSet?
  std::tie(mLowerFocusPos, mUpperFocusPos) = focusCurveRecordSet->minmaxFocusPos();

  // IDEA: The best fiting curve type could later also be determined automatically...
  mFocusCurveType = FocusCurveTypeT::fromFittingCurve(curveFitParms.getFittingCurveType());
  
  
  CurveParmsT curveParms;

  std::vector<PointFT> fitValues;
  std::vector<PointWithResidualT> outlierValues;

  auto dataPoints = *focusCurveRecordSet | boost::adaptors::transformed([this](const auto & r) {
									  float focusMeasure = r->getFocusMeasure(mFocusMeasureType);
									  int absFocusPos = r->getCurrentAbsoluteFocusPos();
									  return PointFT(absFocusPos, focusMeasure);
									});
  // TODO: try-catch??
  mCurveParms = CurveFitAlgorithmT::fitCurve(dataPoints,
					     curveFitParms,
					     & mCurveFitSummary);
  

  // TODO: Also store curveFitSummary in FocusCurveT ??
  // Make a copy of values which were matched
  //fitValues = curveFitSummary.curveDataPoints;
  
  // Make a copy of outliers
  //outlierValues = curveFitSummary.outliers;

  // TODO: Need factory to create a "generic" "FocusCurveFunctionT"(?) which takes CurveParmsT, mFocusCurveType...
  // MathFunctionsT::hyperbolic(x, a, b, c, d);
  mFocusCurveFunction = CurveFunctionFactoryT::getInstance(curveFitParms.getFittingCurveType(), mCurveParms);

  mDateTime = std::time(nullptr);
}

FocusCurveT::~FocusCurveT() {
}

float FocusCurveT::getLowerFocusPos() const {
  return mLowerFocusPos;
}

float FocusCurveT::getUpperFocusPos() const {
  return mUpperFocusPos;
}

float FocusCurveT::getBestAbsFocusPos() const {
  return mFocusCurveFunction->min().x();
}

float FocusCurveT::getRelativeFocusPosBoundary() const {

  // Calculate focus pos boundaries relative to curve center (best focus pos)
  float bestAbsFocusPos = getBestAbsFocusPos();
  float deltaLower = std::abs(bestAbsFocusPos - mLowerFocusPos);
  float deltaUpper = std::abs(mUpperFocusPos - bestAbsFocusPos);

  return std::max(deltaLower, deltaUpper);
}

const CurveParmsT & FocusCurveT::getCurveParms() const {
  return mCurveParms;
}

float FocusCurveT::calcFocusMeasureByFocusPosition(float focusPosition) const {
  // TODO: Check the boundaries??
  
  return mFocusCurveFunction->f(focusPosition);
}

// TODO: static version? -> pass in focusCurveFunction
std::vector<float> FocusCurveT::calcFocusPositionByFocusMeasure(std::shared_ptr<const CurveFunctionT> curveFunction, float focusMeasure) {
  std::vector<float> results;
  
  // TODO: Check the boundaries??
  // TODO: Case when no value...
  
  float estimatedPos = curveFunction->f_inv(focusMeasure);
  
  if (estimatedPos < std::numeric_limits<float>::epsilon()) {
    // Only one value
    results.push_back(estimatedPos);
  } else {
    // Because of the curve shape - we need to consder both values...
    //TODO: Can we assume that here always? Ot shouldn't that be done by MathT::<function>...?
    results.push_back(+estimatedPos);    
    results.push_back(-estimatedPos);
  }
  
  return results;
}

std::vector<float> FocusCurveT::calcFocusPositionByFocusMeasure(float focusMeasure) const {
  return FocusCurveT::calcFocusPositionByFocusMeasure(mFocusCurveFunction, focusMeasure);
}

std::time_t FocusCurveT::getDateTime() const {
  return mDateTime;
}

FocusMeasureTypeT::TypeE FocusCurveT::getFocusMeasureType() const {
  return mFocusMeasureType;
}

FocusCurveTypeT::TypeE FocusCurveT::getFocusCurveType() const {
  return mFocusCurveType;
}

const CurveFitSummaryT & FocusCurveT::getCurveFitSummary() const {
  return mCurveFitSummary;
}

std::ostream &
FocusCurveT::print(std::ostream & os) const {
  // TODO
  os << "--- FocusCurveT ---" << std::endl
     << "   > Focus measure type: " << FocusMeasureTypeT::asStr(mFocusMeasureType) << std::endl
     << "   > Focus measure limit: " << mFocusMeasureLimit << std::endl
     << "   > Fitting curve type: " << FocusCurveTypeT::asStr(mFocusCurveType) << std::endl
     << "   > Curve parms: " << mCurveParms << std::endl
     << "   > [minPos, maxPos]=[" << mLowerFocusPos << ", " << mUpperFocusPos << "]" << std::endl;
  
	return os;
}

std::ostream & operator<<(std::ostream & os, const FocusCurveT & focusCurve) {
	return focusCurve.print(os);
}




