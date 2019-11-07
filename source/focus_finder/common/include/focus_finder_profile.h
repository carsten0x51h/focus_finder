/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_PROFILE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_PROFILE_H_

#include <string>

#include "size.h"
#include "centroid_type.h"
#include "exception.h"
#include "focus_finder_factory.h"
#include "focus_measure_type.h"

DEF_Exception(FocusFinderProfile);

class FocusFinderProfileT {
public:
	static const std::string DEFAULT;

	FocusFinderProfileT();
	~FocusFinderProfileT();

	const std::string & getName() const;
	void setName(const std::string & name);

	const std::string & getCameraName() const;
	void setCameraName(const std::string & cameraName);

//	const std::string & getCoolerName() const;
//	void setCoolerName(const std::string & coolerName);

	const std::string & getFocusName() const;
	void setFocusName(const std::string & focusName);

	const std::string & getFilterName() const;
	void setFilterName(const std::string & filterName);

	const SizeT<unsigned int> & getStarWindowSize() const;
	void setStarWindowSize(const SizeT<unsigned int> & starWindowSize);
  
  FocusMeasureTypeT::TypeE getLimitFocusMeasureType() const;
  void setLimitFocusMeasureType(FocusMeasureTypeT::TypeE limitFocusMeasureType);

  FocusMeasureTypeT::TypeE getCurveFocusMeasureType() const;
  void setCurveFocusMeasureType(FocusMeasureTypeT::TypeE curveFocusMeasureType);
  
	float getStarDetectionSnrBoundary() const;
	void setStarDetectionSnrBoundary(float starDetectionSnrBoundary);

	bool hasCalibrationData() const;
    // ... getCalibrationData() const;
    //void setCalibrationData(...);

	static const FocusFinderProfileT & defaults();
	static const FocusFinderProfileT sDefaultProfile;

	/**
	 * Throws if not found.
	 */
	static FocusFinderProfileT load(const std::string & fullProfilePath);

	/**
	 * Throws if save fails.
	 */
	static void save(const std::string & fullProfilePath,
			const FocusFinderProfileT & profile);

private:
	// Profile properties
	std::string mName;

	// Devices used by this profile
	std::string mCameraName;
//	std::string mCoolerName;
	std::string mFocusName;
	std::string mFilterName;

	// Camera properties
	SizeT<float> mCameraPixelSize; // Unit = um

	// Telescope
	unsigned int mFocalLengthTelescope; // Unit = mm

	// Cooler
	bool mEnableCooler;
	int mExposureTemperature; // °C

	// Filter - Which filter should be used while focusing (enable by checkbox, first...?)
	bool mSetSpecificFilter;
	int mFocusingFilterPos;

	// Calibration settings
	SizeT<unsigned int> mStarWindowSize;

  FocusMeasureTypeT::TypeE mLimitFocusMeasureType;
  FocusMeasureTypeT::TypeE mCurveFocusMeasureType;

	float mStarDetectionSnrBoundary;

	CentroidTypeT::TypeE mCentroidType;
//	FocusFitnessStrategyT::TypeE - HfdT, FwhmT, HfdT+FwhmT, ??;
	bool mStarAutoTracking;

	FocusFinderStrategyT::TypeE mFocusFinderStrategy; // TODO: Actually, this is more a curve type which is used to fit the measured values...

	// TODO: There are probably settings which are specific for the respective "focus finder strategy"
//	e.g.
//	mMaxHfd / mMaxFwhm / limiting HFD?;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_PROFILE_H_ */
