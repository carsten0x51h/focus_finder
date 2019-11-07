#include "include/focus_finder_profile.h"
#include "include/logging.h"
#include "include/device.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

//#include <bits/stdc++.h>
//#include <boost/algorithm/string.hpp>


const std::string FocusFinderProfileT::DEFAULT = "Default";

const FocusFinderProfileT FocusFinderProfileT::sDefaultProfile;

const FocusFinderProfileT & FocusFinderProfileT::defaults() {
	return sDefaultProfile;
}

FocusFinderProfileT::FocusFinderProfileT() :
		mName(DEFAULT),
		mCameraName(DeviceT::NONE),
		//mCoolerName (DeviceT::NONE),
		mFocusName(DeviceT::NONE),
		mFilterName(DeviceT::NONE),
		mStarWindowSize(33, 33),
		mLimitFocusMeasureType(FocusMeasureTypeT::HFD),
		mCurveFocusMeasureType(FocusMeasureTypeT::FWHM_VERT),
		mStarDetectionSnrBoundary(1.0f) {
	LOG(debug)
	<< "FocusFinderProfileT::FocusFinderProfileT()..." << std::endl;
}

FocusFinderProfileT::~FocusFinderProfileT() {
	LOG(debug)
	<< "FocusFinderProfileT::~FocusFinderProfileT()..." << std::endl;
}

const std::string & FocusFinderProfileT::getName() const {
	return mName;
}

void FocusFinderProfileT::setName(const std::string & name) {
	mName = name;
}

const std::string & FocusFinderProfileT::getCameraName() const {
	return mCameraName;
}
void FocusFinderProfileT::setCameraName(const std::string & cameraName) {
	mCameraName = cameraName;
}

//const std::string & FocusFinderProfileT::getCoolerName() const {
//	return mCoolerName;
//}
//void FocusFinderProfileT::setCoolerName(const std::string & coolerName) {
//	mCoolerName = coolerName;
//}

const std::string & FocusFinderProfileT::getFocusName() const {
	return mFocusName;
}
void FocusFinderProfileT::setFocusName(const std::string & focusName) {
	mFocusName = focusName;
}


const std::string & FocusFinderProfileT::getFilterName() const {
	return mFilterName;
}
void FocusFinderProfileT::setFilterName(const std::string & filterName) {
	mFilterName = filterName;
}


const SizeT<unsigned int> & FocusFinderProfileT::getStarWindowSize() const {
	return mStarWindowSize;
}
void FocusFinderProfileT::setStarWindowSize(const SizeT<unsigned int> & starWindowSize) {
	mStarWindowSize = starWindowSize;
}

FocusMeasureTypeT::TypeE FocusFinderProfileT::getLimitFocusMeasureType() const {
  return mLimitFocusMeasureType;
}
void FocusFinderProfileT::setLimitFocusMeasureType(FocusMeasureTypeT::TypeE limitFocusMeasureType) {
  mLimitFocusMeasureType = limitFocusMeasureType;
}

FocusMeasureTypeT::TypeE FocusFinderProfileT::getCurveFocusMeasureType() const {
  return mCurveFocusMeasureType;
}

void FocusFinderProfileT::setCurveFocusMeasureType(FocusMeasureTypeT::TypeE curveFocusMeasureType) {
  mCurveFocusMeasureType = curveFocusMeasureType;
}


float FocusFinderProfileT::getStarDetectionSnrBoundary() const {
	return mStarDetectionSnrBoundary;
}
void FocusFinderProfileT::setStarDetectionSnrBoundary(float starDetectionSnrBoundary) {
	mStarDetectionSnrBoundary = starDetectionSnrBoundary;
}

bool FocusFinderProfileT::hasCalibrationData() const {
    // TODO: Implement!
    return true;
}
// TODO: Implement...
// ... getCalibrationData() const;
//void setCalibrationData(...);


void FocusFinderProfileT::save(const std::string & fullProfilePath, const FocusFinderProfileT & profile) {
  boost::property_tree::ptree pt;

  try {
    pt.put<std::string>("general.name", profile.getName());
    pt.put<std::string>("devices.camera", profile.getCameraName());
    pt.put<std::string>("devices.focus", profile.getFocusName());
    pt.put<std::string>("devices.filter", profile.getFilterName());
    // TODO: Extend

    write_ini(fullProfilePath, pt);
  } catch (boost::property_tree::ini_parser_error & exc) {
    throw FocusFinderProfileExceptionT(exc.what());
  }
}

FocusFinderProfileT FocusFinderProfileT::load(const std::string & fullProfilePath) {

  FocusFinderProfileT profile;

  try {
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(fullProfilePath, pt);

    profile.setName(pt.get<std::string>("general.name"));
    profile.setCameraName(pt.get<std::string>("devices.camera"));
    profile.setFocusName(pt.get<std::string>("devices.focus"));
    profile.setFilterName(pt.get<std::string>("devices.filter"));
    // TODO: Extend

  } catch (boost::property_tree::ini_parser_error & exc) {
    throw FocusFinderProfileExceptionT(exc.what());
  }

  return profile;
}
