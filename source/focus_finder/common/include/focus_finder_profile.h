/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c.schmitt51h@gmail.com>
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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_PROFILE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_PROFILE_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_PROFILE_H_

#include <string>


#include "exception.h"
#include "focus_finder_factory.h"


#include "size.h"
#include "size_ptree_translator.h"

#include "focus_measure_type.h"
#include "focus_measure_type_ptree_translator.h"

#include "centroid_type.h"
#include "centroid_type_ptree_translator.h"

#include "focus_curve_record_set.h"

#include "focus_finder_calibration.h"


DEF_Exception(FocusFinderProfile);

class FocusFinderProfileT {
public:
    static const std::string DEFAULT;

    FocusFinderProfileT();

    ~FocusFinderProfileT();

    [[nodiscard]] size_t getVersion() const;

    void setVersion(size_t version);

    [[nodiscard]] const std::string &getName() const;

    void setName(const std::string &name);

    [[nodiscard]] const TimestampT &getLastChanged() const;

    void setLastChanged(const TimestampT &lastChanged);

    [[nodiscard]] const std::string &getDescription() const;

    void setDescription(const std::string &description);

    /**
     * Camera settings
     */
    [[nodiscard]] std::string getCameraDeviceName() const;

    void setCameraDeviceName(const std::string &cameraDeviceName);

    [[nodiscard]] const SizeT<float> &getCameraPixelSize() const;

    void setCameraPixelSize(const SizeT<float> &cameraPixelSize);

    // TODO: Map of "custom" device specific properties which should be set to a corresponding value - e.g. PORT of an USB device...


    /**
     * Focus settings
     */
    [[nodiscard]] std::string getFocusDeviceName() const;

    void setFocusDeviceName(const std::string &focusDeviceName);

    /**
     * Filter settings
     */
    [[nodiscard]] std::string getFilterDeviceName() const;

    void setFilterDeviceName(const std::string &filterDeviceName);


    /**
     * Calibration settings
     * --------------------
     *
     * 1. Imaging
     */
    [[nodiscard]] const SizeT<unsigned int> &getStarWindowSize() const;

    void setStarWindowSize(const SizeT<unsigned int> &starWindowSize);

    [[nodiscard]] CentroidTypeT::TypeE getCentroidMethod() const;

    void setCentroidMethod(CentroidTypeT::TypeE centroidMethod);

    [[nodiscard]] bool getEnableStarAutoTracking() const;

    void setEnableStarAutoTracking(bool enableStarAutoTracking);

    [[nodiscard]] size_t getNumFramesPerFocusPos() const;

    void setNumFramesPerFocusPos(size_t numFramesPerFocusPos);

    [[nodiscard]] size_t getStarDetectionSnrBoundary() const;

    void setStarDetectionSnrBoundary(float starDetectionSnrBoundary);

    [[nodiscard]] int getFocusingFilterPos() const;

    void setFocusingFilterPos(int focusingFilterPos);

    [[nodiscard]] bool getEnableCameraCooler() const;

    void setEnableCameraCooler(bool enableCameraCooler);

    [[nodiscard]] int getCameraCoolerTargetTemperature() const;

    void setCameraCoolerTargetTemperature(int cameraCoolerTargetTemperature);

    [[nodiscard]] std::chrono::duration<float> getExposureTime() const;

    void setExposureTime(std::chrono::duration<float> exposureTime);


    /**
     * 2. Focus curve recording
     */
    [[nodiscard]] FocusMeasureTypeT::TypeE getLimitFocusMeasureType() const;

    void setLimitFocusMeasureType(FocusMeasureTypeT::TypeE limitFocusMeasureType);

    [[nodiscard]] FocusMeasureTypeT::TypeE getCurveFocusMeasureType() const;

    void setCurveFocusMeasureType(FocusMeasureTypeT::TypeE curveFocusMeasureType);

    [[nodiscard]] int getStepSize() const;

    void setStepSize(int stepSize);

    [[nodiscard]] size_t getNumberCurvesToRecord() const;

    void setNumberCurvesToRecord(size_t numberCurvesToRecord);

    [[nodiscard]] float getFocusMeasureLimit() const;

    void setFocusMeasureLimit(float focusMeasureLimit);


    /**
     * 3. Focus curve matching
     */
    [[nodiscard]] CurveFitParmsT getFocusCurveMatchingParms() const;

    void setFocusCurveMatchingParms(const CurveFitParmsT &focusCurveMatchingParms);


    /**
     * Focus finder calibration data
     */
    [[nodiscard]] std::shared_ptr<FocusFinderCalibrationT> getFocusFinderCalibration() const;

    void setFocusFinderCalibration(std::shared_ptr<FocusFinderCalibrationT> focusFinderCalibration);

    [[nodiscard]] bool hasCalibrationData() const;


    /**
     * Throws if not found.
     */
    static FocusFinderProfileT load(const std::string &fullProfilePath, const std::filesystem::path &lightFramePath);

    /**
     * Throws if save fails.
     */
    static void save(const std::string &fullProfilePath,
                     const std::filesystem::path &lightFramePath,
                     const FocusFinderProfileT &profile);

    std::ostream &print(std::ostream &os, size_t indent = 0) const;


    static const FocusFinderProfileT &defaults();

private:
    static const FocusFinderProfileT sDefaultProfile;

    /**
     * Profile meta data
     */
    size_t mVersion;
    std::string mName;
    TimestampT mLastChanged;
    std::string mDescription;


    /**
     * Camera settings
     */
    std::string mCameraDeviceName;
    SizeT<float> mCameraPixelSize; // Unit = um


    /**
     * Focus settings
     */
    std::string mFocusDeviceName;


    /**
     * Filter settings
     */
    std::string mFilterDeviceName;


    /**
     * Calibration - Imaging
     */
    SizeT<unsigned int> mStarWindowSize;
    CentroidTypeT::TypeE mCentroidMethod;
    bool mEnableStarAutoTracking;
    size_t mNumFramesPerFocusPos;
    float mStarDetectionSnrBoundary;
    int mFocusingFilterPos;
    bool mEnableCameraCooler;
    int mCameraCoolerTargetTemperature;
    std::chrono::duration<float> mExposureTime;


    /**
     * Calibration - Focus curve recording
     */
    FocusMeasureTypeT::TypeE mLimitFocusMeasureType;
    FocusMeasureTypeT::TypeE mCurveFocusMeasureType;
    int mStepSize;
    int mNumberCurvesToRecord;
    float mFocusMeasureLimit;


    /**
     * Calibration - Focus curve matching
     */
    CurveFitParmsT mFocusCurveMatchingParms;


    /**
     * Focus finder
     *
     * NOTE / TODO: There are probably settings which are specific for the respective "focus finder strategy".
     *              So it will probably be hard to just "extract" the strategy and create it dynamically on
     *              request using a factory. In case a different configuration is required, new / corresponding
     *              GUI panels in the settings dialog would be required. Therefore, to keep it simple,
     *              the "focus finder strategy" whould be hardcoded (i.e. not selectable) at all for now.
     */
    //FocusFinderStrategyT::TypeE mFocusFinderStrategy; //< Defines which focus finder strategy will be used to find the focus


    /**
     * Calibration data
     *
     * TODO: We may store only the focus curve function parms instead...
     * TODO: Maybe rename this to "calibration data"...
     *       In case there is no calibration data, the container is empty.
     */
    std::shared_ptr<FocusFinderCalibrationT> mFocusFinderCalibration;


    /**
     * Telescope
     *
     * TODO: Maybe later..
     */
    //unsigned int mFocalLengthTelescope; // Unit = mm
};

// TODO: Remove... member access no longer required since print() is now used..
std::ostream &operator<<(std::ostream &os, const FocusFinderProfileT &focusFinderProfile);

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_PROFILE_H_ */
