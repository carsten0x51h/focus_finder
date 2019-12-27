#include "include/focus_finder_profile.h"
#include "include/focus_measure_type.h"
#include "include/logging.h"
#include "include/device.h"

#include "include/focus_curve_record_set.h"
#include "include/timestamp.h"

#include "include/centroid_type_ptree_translator.h"
#include "include/duration_ptree_translator.h"
#include "include/timestamp_ptree_translator.h"

#include <chrono>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

//#include <bits/stdc++.h>
//#include <boost/algorithm/string.hpp>

using namespace std::chrono_literals;

const std::string FocusFinderProfileT::DEFAULT = "Default";

const FocusFinderProfileT FocusFinderProfileT::sDefaultProfile;


FocusFinderProfileT::FocusFinderProfileT() :
  mName(DEFAULT),
  mLastChanged(std::chrono::high_resolution_clock::now()),
		
  mCameraDeviceName(DeviceT::NONE),
  mCameraPixelSize(0.0F, 0.0F),
		
  mFocusDeviceName(DeviceT::NONE),

  mFilterDeviceName(DeviceT::NONE),

  mStarWindowSize(33, 33),
  mCentroidMethod(CentroidTypeT::IWC),
  mEnableStarAutoTracking(true),
  mNumFramesPerFocusPos(3),
  mStarDetectionSnrBoundary(1.0F),
  mFocusingFilterPos(-1), // TODO: OK?
  mEnableCameraCooler(false),
  mCameraCoolerTargetTemperature(-10),
  mFocusCurveRecordingExposureTime(1s),

  mLimitFocusMeasureType(FocusMeasureTypeT::HFD),
  mCurveFocusMeasureType(FocusMeasureTypeT::FWHM_AVERAGE),
  mStepSize(1000), // TODO: Should there be a "valid" default? Or better 0?
  mNumberCurvesToRecord(5),
  mFocusMeasureLimit(10.0F) { // TODO: Depends on focus measure and system...

	LOG(debug)
	<< "FocusFinderProfileT::FocusFinderProfileT()..." << std::endl;
}

FocusFinderProfileT::~FocusFinderProfileT() {
	LOG(debug)
	<< "FocusFinderProfileT::~FocusFinderProfileT()..." << std::endl;
}


/**
 * Profile basics
 */
const std::string & FocusFinderProfileT::getName() const {
	return mName;
}

void FocusFinderProfileT::setName(const std::string & name) {
	mName = name;
}

const TimestampT & FocusFinderProfileT::getLastChanged() const {
	return mLastChanged;
}

void FocusFinderProfileT::setLastChanged(const TimestampT & lastChanged) {
	mLastChanged = lastChanged;
}


const std::string & FocusFinderProfileT::getDescription() const {
	return mDescription;
}

void FocusFinderProfileT::setDescription(const std::string & description) {
	mDescription = description;
}




/**
 * Camera settings
 */
std::string FocusFinderProfileT::getCameraDeviceName() const {
	return mCameraDeviceName;
}
void FocusFinderProfileT::setCameraDeviceName(const std::string & cameraDeviceName) {
	mCameraDeviceName = cameraDeviceName;
}

const SizeT<float> & FocusFinderProfileT::getCameraPixelSize() const {
  return mCameraPixelSize;
}
void FocusFinderProfileT::setCameraPixelSize(const SizeT<float> & cameraPixelSize) {
  mCameraPixelSize = cameraPixelSize;
}



/**
 * Focus settings
 */
std::string FocusFinderProfileT::getFocusDeviceName() const {
	return mFocusDeviceName;
}
void FocusFinderProfileT::setFocusDeviceName(const std::string & focusDeviceName) {
	mFocusDeviceName = focusDeviceName;
}


/**
 * Filter settings
 */
std::string FocusFinderProfileT::getFilterDeviceName() const {
  return mFilterDeviceName;
}
void FocusFinderProfileT::setFilterDeviceName(const std::string & filterDeviceName) {
  mFilterDeviceName = filterDeviceName;
}


/**
 * Calibration settings
 * --------------------
 *
 * 1. Imaging
 */
const SizeT<unsigned int> & FocusFinderProfileT::getStarWindowSize() const {
  return mStarWindowSize;
}
void FocusFinderProfileT::setStarWindowSize(const SizeT<unsigned int> & starWindowSize) {
  mStarWindowSize = starWindowSize;
}

CentroidTypeT::TypeE FocusFinderProfileT::getCentroidMethod() const {
  return mCentroidMethod;
}
void FocusFinderProfileT::setCentroidMethod(CentroidTypeT::TypeE centroidMethod) {
  mCentroidMethod = centroidMethod;
}

bool FocusFinderProfileT::getEnableStarAutoTracking() const {
  return mEnableStarAutoTracking;
}
void FocusFinderProfileT::setEnableStarAutoTracking(bool enableStarAutoTracking) {
  mEnableStarAutoTracking = enableStarAutoTracking;
}

size_t FocusFinderProfileT::getNumFramesPerFocusPos() const {
  return mNumFramesPerFocusPos;
}
void FocusFinderProfileT::setNumFramesPerFocusPos(size_t numFramesPerFocusPos) {
  mNumFramesPerFocusPos = numFramesPerFocusPos;
}

size_t FocusFinderProfileT::getStarDetectionSnrBoundary() const {
  return mStarDetectionSnrBoundary;
}
void FocusFinderProfileT::setStarDetectionSnrBoundary(float starDetectionSnrBoundary) {
  mStarDetectionSnrBoundary = starDetectionSnrBoundary;
}


int FocusFinderProfileT::getFocusingFilterPos() const {
  return mFocusingFilterPos;
}
void FocusFinderProfileT::setFocusingFilterPos(int focusingFilterPos) {
  mFocusingFilterPos = focusingFilterPos;
}


bool FocusFinderProfileT::getEnableCameraCooler() const {
  return mEnableCameraCooler;
}
void FocusFinderProfileT::setEnableCameraCooler(bool enableCameraCooler) {
  mEnableCameraCooler = enableCameraCooler;
}


int FocusFinderProfileT::getCameraCoolerTargetTemperature() const {
  return mCameraCoolerTargetTemperature;
}
void FocusFinderProfileT::setCameraCoolerTargetTemperature(int cameraCoolerTargetTemperature) {
  mCameraCoolerTargetTemperature = cameraCoolerTargetTemperature;
}

std::chrono::duration<float> FocusFinderProfileT::getFocusCurveRecordingExposureTime() const {
  return mFocusCurveRecordingExposureTime;
}
void FocusFinderProfileT::setFocusCurveRecordingExposureTime(std::chrono::duration<float> focusCurveRecordingExposureTime) {
  mFocusCurveRecordingExposureTime = focusCurveRecordingExposureTime;
}


/**
 * 2. Focus curve recording
 */
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

int FocusFinderProfileT::getStepSize() const {
  return mStepSize;
}
void FocusFinderProfileT::setStepSize(int stepSize) {
  mStepSize = stepSize;
}

size_t FocusFinderProfileT::getNumberCurvesToRecord() const {
  return mNumberCurvesToRecord;
}
void FocusFinderProfileT::setNumberCurvesToRecord(size_t numberCurvesToRecord) {
  mNumberCurvesToRecord = numberCurvesToRecord;
}

float FocusFinderProfileT::getFocusMeasureLimit() const {
  return mFocusMeasureLimit;
}
void FocusFinderProfileT::setFocusMeasureLimit(float focusMeasureLimit) {
  mFocusMeasureLimit = focusMeasureLimit;
}


/**
 * 3. Focus curve matching
 */
CurveFitParmsT FocusFinderProfileT::getFocusCurveMatchingParms() const {
  return mFocusCurveMatchingParms;
}
void FocusFinderProfileT::setFocusCurveMatchingParms(const CurveFitParmsT & focusCurveMatchingParms) {
  mFocusCurveMatchingParms = focusCurveMatchingParms;
}



/**
 * Focus finder calibration data
 */
std::shared_ptr<FocusFinderCalibrationT> FocusFinderProfileT::getFocusFinderCalibration() const {
  return mFocusFinderCalibration;
}

void FocusFinderProfileT::setFocusFinderCalibration(std::shared_ptr<FocusFinderCalibrationT> focusFinderCalibration) {
  mFocusFinderCalibration = focusFinderCalibration;
}

bool FocusFinderProfileT::hasCalibrationData() const {
  return (mFocusFinderCalibration->numRecordSets() > 0);
}


FocusFinderProfileT FocusFinderProfileT::load(const std::string & fullProfilePath) {
    
  FocusFinderProfileT profile;

  try {
    boost::property_tree::ptree pt;
    boost::property_tree::xml_parser::read_xml(fullProfilePath, pt);

    /**
     * Profile basics
     */
    profile.setName(pt.get("profile.<xmlattr>.name", defaults().getName()));
    profile.setLastChanged(pt.get("profile.<xmlattr>.last_changed", defaults().getLastChanged()));
    profile.setDescription(pt.get("profile.<xmlattr>.description", defaults().getDescription()));

    /**
     * Devices
     *
     * 1. Camera
     */
    const std::string cameraSettingsPath = "profile.settings.camera_settings";

    profile.setCameraDeviceName(pt.get(cameraSettingsPath + ".camera_device_name", defaults().getCameraDeviceName()));
    profile.setCameraPixelSize(pt.get(cameraSettingsPath + ".camera_pixel_size", defaults().getCameraPixelSize()));

    
    /**
     * 2. Focus
     */
    profile.setFocusDeviceName(pt.get("profile.settings.focus_settings.focus_device_name", defaults().getFocusDeviceName()));

    
    /**
     * 3. Filter
     */
    profile.setFilterDeviceName(pt.get("profile.settings.filter_settings.filter_device_name", defaults().getFocusDeviceName()));


    
    /**
     * Calibration
     *
     * 1. Imaging
     *
     * TODO: Exposure time automatically determine best exposure time?
     */
    // TODO: Make imagingPath static...
    const std::string imagingPath = "profile.settings.calibration_settings.imaging";

    profile.setStarWindowSize(pt.get(imagingPath + ".star_window_size", defaults().getStarWindowSize()));
    profile.setCentroidMethod(pt.get(imagingPath + ".imaging.centroid_method", defaults().getCentroidMethod()));
    profile.setEnableStarAutoTracking(pt.get(imagingPath + ".enable_star_auto_tracking", defaults().getEnableStarAutoTracking()));
    profile.setNumFramesPerFocusPos(pt.get(imagingPath + ".num_frames_per_focus_pos", defaults().getNumFramesPerFocusPos()));
    profile.setStarDetectionSnrBoundary(pt.get(imagingPath + ".star_detection_snr_boundary", defaults().getStarDetectionSnrBoundary()));
    profile.setFocusingFilterPos(pt.get(imagingPath + ".focusing_filter_pos", defaults().getFocusingFilterPos()));  // NOTE: -1 = do not change / set
    profile.setEnableCameraCooler(pt.get(imagingPath + ".enable_camera_cooler", defaults().getEnableCameraCooler()));
    profile.setCameraCoolerTargetTemperature(pt.get(imagingPath + ".camera_cooler_target_temperature", defaults().getCameraCoolerTargetTemperature()));
    profile.setFocusCurveRecordingExposureTime(pt.get(imagingPath + ".focus_curve_recording_exposure_time", defaults().getFocusCurveRecordingExposureTime()));

    
    /**
     * 2. Focus curve recording
     */
    // TODO: Make focusCurveRecordingPath static...
    const std::string focusCurveRecordingPath = "profile.settings.calibration_settings.focus_curve_recording";
    
    profile.setLimitFocusMeasureType(pt.get(focusCurveRecordingPath + ".limit_focus_measure_type", defaults().getLimitFocusMeasureType()));
    profile.setCurveFocusMeasureType(pt.get(focusCurveRecordingPath + ".curve_focus_measure_type", defaults().getCurveFocusMeasureType()));
    profile.setStepSize(pt.get<int>(focusCurveRecordingPath + ".step_size")); // Explicitely no default
    profile.setNumberCurvesToRecord(pt.get(focusCurveRecordingPath + ".num_curves_to_record", defaults().getNumberCurvesToRecord()));
    profile.setFocusMeasureLimit(pt.get(focusCurveRecordingPath + ".focus_measure_limit", defaults().getFocusMeasureLimit()));
    
    
    /**
     * 3. Focus curve matching
     */
    CurveFitParmsT curveFitParms = CurveFitParmsT::load(pt);
    
    profile.setFocusCurveMatchingParms(curveFitParms);
    
    
    /**
     * Calibration data
     *
     * Another way to store would be to just pass the subtree:
     *
     * See https://stackoverflow.com/questions/29521533/creating-a-boost-property-tree-recursively
     *
     *   ptree insertProjectNode(ITreeNode const& node)
     *
     * This way we can write the record stuff to its own property tree. It can later be 
     * integrated / added to the profile property tree ... or not - this can be decieded later - e.g.
     *
     *   ptree ptSub = profile.getFocusCurveRecordSetContainer()->getPropertyTree();
     *   pt.add_child("calibration.data", ptSub);
     *
     */

    profile.setFocusFinderCalibration(FocusFinderCalibrationT::load(pt.get_child("profile.calibration"), curveFitParms));

    
    //
    // Add further fields which should need to be loaded here...
    //

    
  } catch (boost::property_tree::xml_parser_error & exc) {
    throw FocusFinderProfileExceptionT(exc.what());
  }

  return profile;
}


void FocusFinderProfileT::save(const std::string & fullProfilePath, const FocusFinderProfileT & profile) {
  boost::property_tree::ptree pt;

  // TODO: Need translator for std::chrono::duration<float>  --> 1.5s
  
  try {
    /**
     * Profile basics
     */
    pt.put<std::string>("profile.<xmlattr>.name", profile.getName());
    pt.put<TimestampT>("profile.<xmlattr>.last_changed", profile.getLastChanged());
    pt.put<std::string>("profile.<xmlattr>.description", profile.getDescription());

    /**
     * Devices
     *
     * 1. Camera
     */
    const std::string cameraSettingsPath = "profile.settings.camera_settings";

    pt.put<std::string>(cameraSettingsPath + ".camera_device_name", profile.getCameraDeviceName());
    pt.put<SizeT<float> >(cameraSettingsPath + ".camera_pixel_size", profile.getCameraPixelSize());

    
    /**
     * 2. Focus
     */
    pt.put<std::string>("profile.settings.focus_settings.focus_device_name", profile.getFocusDeviceName());

    
    /**
     * 3. Filter
     */
    pt.put<std::string>("profile.settings.filter_settings.filter_device_name", profile.getFilterDeviceName());


    
    /**
     * Calibration
     *
     * 1. Imaging
     *
     * TODO: Exposure time automatically determine best exposure time?
     */
    const std::string imagingPath = "profile.settings.calibration_settings.imaging";

    pt.put<SizeT<unsigned int> >(imagingPath + ".star_window_size", profile.getStarWindowSize());
    pt.put<CentroidTypeT::TypeE>(imagingPath + ".imaging.centroid_method", profile.getCentroidMethod());
    pt.put<bool>(imagingPath + ".enable_star_auto_tracking", profile.getEnableStarAutoTracking());
    pt.put<int>(imagingPath + ".num_frames_per_focus_pos", profile.getNumFramesPerFocusPos());
    pt.put<float>(imagingPath + ".star_detection_snr_boundary", profile.getStarDetectionSnrBoundary());
    pt.put<int>(imagingPath + ".focusing_filter_pos", profile.getFocusingFilterPos()); // NOTE: -1 = do not change / set
    pt.put<bool>(imagingPath + ".enable_camera_cooler", profile.getEnableCameraCooler());
    pt.put<int>(imagingPath + ".camera_cooler_target_temperature", profile.getCameraCoolerTargetTemperature());

    pt.put<std::chrono::duration<float> >(imagingPath + ".focus_curve_recording_exposure_time", profile.getFocusCurveRecordingExposureTime());


    /**
     * 2. Focus curve recording
     */
    const std::string focusCurveRecordingPath = "profile.settings.calibration_settings.focus_curve_recording";

    pt.put<typename FocusMeasureTypeT::TypeE>(focusCurveRecordingPath + ".limit_focus_measure_type", profile.getLimitFocusMeasureType());
    pt.put<typename FocusMeasureTypeT::TypeE>(focusCurveRecordingPath + ".curve_focus_measure_type", profile.getCurveFocusMeasureType());
    pt.put<int>(focusCurveRecordingPath + ".step_size", profile.getStepSize());
    pt.put<size_t>(focusCurveRecordingPath + ".num_curves_to_record", profile.getNumberCurvesToRecord());
    pt.put<float>(focusCurveRecordingPath + ".focus_measure_limit", profile.getFocusMeasureLimit());

    
    /**
     * 3. Focus curve matching
     */
    CurveFitParmsT::save(pt, profile.getFocusCurveMatchingParms());
    
    
    /**
     * Calibration data
     *
     * Another way to store would be to just pass the subtree:
     *
     * See https://stackoverflow.com/questions/29521533/creating-a-boost-property-tree-recursively
     *
     *   ptree insertProjectNode(ITreeNode const& node)
     *
     * This way we can write the record stuff to its own property tree. It can later be 
     * integrated / added to the profile property tree ... or not - this can be decieded later - e.g.
     *
     *   ptree ptSub = profile.getFocusCurveRecordSetContainer()->getPropertyTree();
     *   pt.add_child("calibration.data", ptSub);
     *
     */
    FocusFinderCalibrationT::save(pt, profile.getFocusFinderCalibration());

    
    
    // Add further fiels which should need to be stored here...

    
    write_xml(fullProfilePath, pt);
    
  } catch (boost::property_tree::xml_parser_error & exc) {
    throw FocusFinderProfileExceptionT(exc.what());
  }
}



std::ostream & FocusFinderProfileT::print(std::ostream & os, size_t indent) const {

  std::string prefix = std::string(indent, ' ');

  /**
   * Basic profile settings
   */
  os << prefix << "---Profile basics---" << std::endl
     << prefix << "Name: " << mName << std::endl
     << prefix << "Last changed: " << mLastChanged << std::endl // TODO: Format?
     << prefix << "Description: " << mDescription << std::endl;
  
  /**
   * Camera settings
   */
  os << std::endl
     << prefix << "---Camera settings---" << std::endl
     << prefix << "Camera device name: " << mCameraDeviceName << std::endl
     << prefix << "Camera pixel size: " << mCameraPixelSize << std::endl;

  /**
   * Focus settings
   */
  os << std::endl
     << prefix << "---Focus settings---" << std::endl
     << prefix << "Focus device name: " << mFocusDeviceName << std::endl;
  
  /**
   * Filter settings
   */
  os << std::endl
     << prefix << "---Filter settings---" << std::endl
     << prefix << "Filter device name: " << mFilterDeviceName << std::endl;

  /**
   * Calibration settings - Imaging
   */
  os << std::endl
     << prefix << "---Calibration settings - Imaging---" << std::endl
     << prefix << "Star window size: " << mStarWindowSize << std::endl
     << prefix << "Centroid method: " << CentroidTypeT::asStr(mCentroidMethod) << std::endl
     << prefix << "Star auto tracking enabled: " << (mEnableStarAutoTracking ? "yes" : "no") << std::endl
     << prefix << "#frames per focus position: " << mNumFramesPerFocusPos << std::endl
     << prefix << "SNR star detection boundary: " << mStarDetectionSnrBoundary << std::endl
     << prefix << "Focusing filter pos: " << mFocusingFilterPos << std::endl
     << prefix << "Camera cooler enabled: " << (mEnableCameraCooler ? "yes" : "no") << std::endl
     << prefix << "Camera cooler target temperature: " << mCameraCoolerTargetTemperature << std::endl
     << prefix << "Focus curve recording exposure time[s]: " <<  mFocusCurveRecordingExposureTime.count() << std::endl;
  

  /**
   * Calibration settings - Focus curve recording
   */
  os << std::endl
     << prefix << "---Calibration settings - Focus curve recording---" << std::endl
     << prefix << "Limit focus measure type: " << FocusMeasureTypeT::asStr(mLimitFocusMeasureType) << std::endl
     << prefix << "Curve focus measure type: " << FocusMeasureTypeT::asStr(mCurveFocusMeasureType) << std::endl
     << prefix << "Focus step size: " << mStepSize << std::endl
     << prefix << "Number of curves to record: " << mNumberCurvesToRecord << std::endl
     << prefix << "Focus measure limit: " << mFocusMeasureLimit << std::endl;

  /**
   * Calibration settings - Focus curve matching
   */
  os << std::endl
     << prefix << "---Calibration settings - Focus curve matching---" << std::endl
     << prefix << "CurveFitParms" << std::endl;

  mFocusCurveMatchingParms.print(os, indent + 4);

  os << std::endl;

  /**
   * Focus finder calibration
   */
  mFocusFinderCalibration->print(os,  indent + 4);

  os << std::endl;

  return os;
}


const FocusFinderProfileT & FocusFinderProfileT::defaults() {
  return sDefaultProfile;
}

std::ostream & operator<<(std::ostream & os, const FocusFinderProfileT & focusFinderProfile) {
	return focusFinderProfile.print(os);
}
