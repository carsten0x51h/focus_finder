/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c [at] lost-infinity.com>
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

#include <memory>
#include <optional>
#include <algorithm>
#include <utility>

#include "include/focus_finder_logic.h"

#include "include/device_manager.h"
#include "include/device_manager_type.h"
#include "include/device_manager_factory.h"

#include "include/fofi_config_manager.h"
#include "include/global_config_manager.h"
#include "include/profile_manager.h"
#include "include/logging.h"
#include "include/snr.h"
#include "include/centroid.h"
#include "include/single_star_detector_algorithm.h"

#include "include/linear_bw_stretch_mapper_function.h"
#include "include/spline_curve_mapper_function.h"
#include "include/image_converter_16to8.h"
#include "include/focus_finder.h"
#include "include/task_executor.h"
#include "include/focus_curve_recorder_logic.h"

#include "include/event_bus.h"
#include "include/events/new_frame_event.h"

FocusFinderLogicT *FocusFinderLogicT::sSelf = nullptr;

void FocusFinderLogicT::init() {
    sSelf = new FocusFinderLogicT;

    logging::trivial::severity_level sev = logging::trivial::debug;
    LoggingT::init(sev, true /*console*/, true /*log file*/);
}

void FocusFinderLogicT::close() {
    delete sSelf;
    sSelf = nullptr;
}

FocusFinderLogicT *FocusFinderLogicT::get() {
    return sSelf;
}

FocusFinderLogicT::FocusFinderLogicT() :
        mCameraDevice(nullptr) {
    LOG(debug)
        << "FocusFinderLogicT::FocusFinderLogicT()..." << std::endl;


    mFoFiConfigManager = std::make_shared<FoFiConfigManagerT>();

    GlobalFocusFinderConfigT globalCfg = mFoFiConfigManager->getGlobalConfigManager()->getConfig();

    DeviceManagerTypeT::TypeE deviceManagerType = globalCfg.getDeviceManagerType();

    LOG(debug) << "DeviceManagerType configured: " << DeviceManagerTypeT::asStr(deviceManagerType) << std::endl;

    // Create the device manager
    mDeviceManager = DeviceManagerFactoryT::getInstance(deviceManagerType);


    boost::property_tree::ptree deviceManagerConfig = globalCfg.getDeviceManagerConfig();
    mDeviceManager->configure(deviceManagerConfig);


    // DEBUG START
    LOG(debug) << "CCD LIST" << std::endl;
    for (const auto &s : mDeviceManager->getDeviceNames(DeviceInterfaceTypeT::CCD)) {
        LOG(debug) << s << std::endl;
    }
    LOG(debug) << "FILTER LIST" << std::endl;
    for (const auto &s : mDeviceManager->getDeviceNames(DeviceInterfaceTypeT::FILTER)) {
        LOG(debug) << s << std::endl;
    }
    LOG(debug) << "FOCUS LIST" << std::endl;
    for (const auto &s : mDeviceManager->getDeviceNames(DeviceInterfaceTypeT::FOCUS)) {
        LOG(debug) << s << std::endl;
    }
    // DEBUG END


    updateProfile();

    // Register at profile manager to get notified if selected profile / device changes...
    mFoFiConfigManager->getProfileManager()->registerActiveProfileChangedListener(
            boost::bind(&FocusFinderLogicT::updateProfile, this)
    );


    initImageMappers();

    mImageConverter16To8 = std::make_shared<ImageConverter16To8T>();

    // TODO: Set default one?! - set black- and white point here - just a HACK - later by panel? Or who should se t the default?...
    mImageConverter16To8->setMapperFunction(
            getMapperFunctionByName("LinearBWStretchMapperFunction"));

    // Init the foocus finder executor
    mFocusFinderExecutor = std::make_shared<TaskExecutorT<FocusFinderT> >();

    // Create the FocusCurveRecorderLogic
    mFocusCurveRecorderLogic = std::make_shared<FocusCurveRecorderLogicT>(*this);
}

FocusFinderLogicT::~FocusFinderLogicT() = default;

std::shared_ptr<CameraInterfaceT> FocusFinderLogicT::getCurrentCamera() {
    auto activeProfile = mFoFiConfigManager->getProfileManager()->getActiveProfile();

    auto cameraDevice = (activeProfile ?
                         mDeviceManager->getDevice(activeProfile->getCameraDeviceName()) :
                         nullptr);

    return (cameraDevice != nullptr ? cameraDevice->getCameraInterface() : nullptr);
}

std::shared_ptr<FocusInterfaceT> FocusFinderLogicT::getCurrentFocus() {
    auto activeProfile = mFoFiConfigManager->getProfileManager()->getActiveProfile();

    auto focusDevice = (activeProfile ?
                        mDeviceManager->getDevice(activeProfile->getFocusDeviceName()) :
                        nullptr);

    return (focusDevice != nullptr ? focusDevice->getFocusInterface() : nullptr);
}

std::shared_ptr<FilterInterfaceT> FocusFinderLogicT::getCurrentFilter() {
    auto activeProfile = mFoFiConfigManager->getProfileManager()->getActiveProfile();

    auto filterDevice = (activeProfile ?
                         mDeviceManager->getDevice(activeProfile->getFilterDeviceName()) :
                         nullptr);

    return (filterDevice != nullptr ? filterDevice->getFilterInterface() : nullptr);
}

std::shared_ptr<DeviceManagerT> FocusFinderLogicT::getDeviceManager() const {
    return mDeviceManager;
}

std::shared_ptr<ProfileManagerT> FocusFinderLogicT::getProfileManager() const {
    return mFoFiConfigManager->getProfileManager();
}

void FocusFinderLogicT::updateProfile() {

    auto newCameraDevice = getCurrentCamera();
    bool hasOldCamera = (mCameraDevice != nullptr);

    if (mCameraDevice == newCameraDevice) {
        return;
    }

    if (hasOldCamera) {
        // Clear (unregister) all previously registered listeners from the old device. NOTE: Do this only if the device has changes! All components will have to register their listeners again anyway after the device has changed.

        //   mCameraDevice->clearListeners();

        // TODO: Do the same in here for focuser and filter as well...
        // IDEA: Each device may call a callback which informs whomever that it's listener was removed...?!

        // TODO: Remove the unregister code from all the components...
        //  -> we could also introduce a more granular deviceChanged() callback/listener (e.g. below)
        // There was already an old device - unregister listener and register to the new one.
        mCameraDevice->unregisterExposureCycleFinishedListener(mExposureCycleFinishedConnection);
    }

    // Register to new device
    if (newCameraDevice) {
        mExposureCycleFinishedConnection = newCameraDevice->registerExposureCycleFinishedListener(
                [&](RectT<unsigned int> roiRect, std::shared_ptr<const ImageT> resultImage, bool lastExposure) {

                    NewFrameEventT newFrameEvent(roiRect, resultImage, lastExposure);

                    EventBusT::getInstance()->publish<void(NewFrameEventT)>(newFrameEvent);

                    // TODO: Implement... need position if selected star (if any)
                    // NOTE: This may move to a sep. class / controller based on data in NewFrameEventT...
//                    size_t centerIdxHorz = std::floor(img.height() / 2);
//                    FwhmT fwhmHorz(ImageSlicerT::slice<SliceDirectionT::HORZ>(img, centerIdxHorz));
//
//                    // NOTE / TODO: This only works if width() is odd
//                    size_t centerIdxVert = std::floor(img.width() / 2);
//                    FwhmT fwhmVert(ImageSlicerT::slice<SliceDirectionT::VERT>(img, centerIdxVert));






                    // TODO: "mLastFrame" needs mutex guard!!!-> or atomic?
                    mLastFrame.setImage(std::move(resultImage));  // null if no image
                    mLastFrame.setRoi(roiRect);        // "empty" - i.e. 0,0,0,0 if not set
                }
        );
    }

    mCameraDevice = newCameraDevice;



    // TODO: Shouldn't getCurrentCamera() return mCameraDevice instead?
    // 	  TODO: Shouldn't mCameraDevice be protected by a lockguard? Because the event which changes it comes from the "profile manager" callback which might be async to a request to getCurrentCamera()....
}

void FocusFinderLogicT::setSelectedRoi(const RectT<unsigned int> &roi) {

    mSelectedRoi = roi; // TODO: Is saving this member inside FocusFinderLogic required?
    //mLastFrame.setRoi(roi); TODO: Should this be set here??? The Frame is not yet here!!! It should only be set when new frame was received!
    //	TODO:Check if subFrame is correct... (isSet() and within valid bounds, otherwise throw.)

    LOG(debug)
        << "FocusFinderLogicT::setSelectedSubFrame... subFrame: "
        << std::get<0>(roi) << ", " << std::get<1>(roi) << ", w: "
        << std::get<2>(roi) << ", h: " << std::get<3>(roi) << std::endl;

    std::shared_ptr<CameraInterfaceT> camera = getCurrentCamera();

    if (camera->isRoiSupported()) {
        camera->setRoi(roi);
    } else {
        // TODO: throw exception ?
    }
}

void FocusFinderLogicT::clearSelectedRoi() {
    LOG(debug)
        << "FocusFinderLogicT::clearSelectedRoi()..." << std::endl;
    mSelectedRoi.clear();

    std::shared_ptr<CameraInterfaceT> camera = getCurrentCamera();

    if (camera->isRoiSupported()) {
        camera->clearRoi();
    }
}

RectT<unsigned int> FocusFinderLogicT::getSelectedRoi() const {
    return mSelectedRoi;
}

std::optional<PointT<float> > FocusFinderLogicT::getLastFocusStarPos() const {
    return mLastFocusStarPos;
}

// TODO: PointT<float> & poi -> PointT<int> & poi ??
// TODO / FIXME: After removing the POI amd clicking into nowhere, still the old star is selected again..
std::optional<PointT<float> > FocusFinderLogicT::findFocusStar(
        const PointT<float> &poi) {

    auto activeProfile = mFoFiConfigManager->getProfileManager()->getActiveProfile();

    if (!activeProfile) {
        LOG(warning) << "No active profile set!" << std::endl;
        return std::nullopt;
    }

    auto img = mLastFrame.getImage();

    if (img == nullptr) {
        LOG(warning)
            << "FocusFinderLogicT::findFocusStar... failed - no frame available."
            << std::endl;

        return std::nullopt;
    }

    // Transform given POI to "sub-frame" coordinates:
    //
    // NOTE: Point "poi" passed in is in "pixmap" coordinates, i.e. in case there is
    //       just a subframe image available, the coordinates need to be transformed.
    //       In case roi is not set, roiX and roiY are just 0. That means we can make
    //       the same transformation always - just in case of a "full frame" it simply
    //       has no effect.
    auto frameBounds = mLastFrame.getBounds().to<int>(); // RectT<unsigned int>

    SingleStarDetectorAlgorithmT singleStarDetectorAlgorithm(activeProfile->getStarDetectionSnrBoundary(), activeProfile->getStarWindowSize());

    PointT<float> subFramePoi(poi.x() - (float) frameBounds.x(), poi.y() - (float) frameBounds.y());

    SingleStarDetectorAlgorithmT::ResultT result = singleStarDetectorAlgorithm.detect(img, subFramePoi);

    // Log the result
    LOG(info) << "FocusFinderLogicT::findFocusStar result - "
              << result
              << std::endl;

    // NOTE: In case of success, the calculated star center coordinate needs to be re-transformed to pixmap coordinates
    if (SingleStarDetectorAlgorithmT::ResultT::StatusT::SINGLE_STAR_DETECTED == result.getStatus()) {
        mLastFocusStarPos.emplace(PointT<float>(result.getStarCenterPos().x() + (float) frameBounds.x(),
                                                result.getStarCenterPos().y() + (float) frameBounds.y()));
    }

    return mLastFocusStarPos;
}

std::shared_ptr<FocusCurveRecorderLogicT> FocusFinderLogicT::getFocusCurveRecorderLogic() {
    return mFocusCurveRecorderLogic;
}

// TODO: Move to mFocusAnalyzer?
const FrameT &FocusFinderLogicT::getLastFrame() const {
    // TODO: Need mutex guard!!!-> or atomic?
    return mLastFrame;
}

void FocusFinderLogicT::initImageMappers() {
    auto linearBWStretchMapper =
            std::make_shared<LinearBWStretchMapperFunctionT>();
    mMapperFunctions.push_back(linearBWStretchMapper);

    auto splineCurveMapper = std::make_shared<SplineCurveMapperFunctionT>();
    mMapperFunctions.push_back(splineCurveMapper);

    // TODO: Add further mappers
    //c2 = new HighContrastCurveT(numSrcValues, numDestValues);
    //c3 = new CustomCurveT();
}

// See https://stackoverflow.com/questions/4986673/c11-rvalues-and-move-semantics-confusion-return-statement/4986802#4986802
std::vector<std::string> FocusFinderLogicT::getMapperFunctionNames() const {
    LOG(debug)
        << "FocusFinderLogicT::getMapperFunctionNames()... mMapperFunctions.size(): "
        << mMapperFunctions.size() << std::endl;

    // See https://stackoverflow.com/questions/33048543/c-how-to-handle-the-returned-value-of-lambda-expression-used-in-for-each-loop
    std::vector<std::string> mapperFunctionNames(mMapperFunctions.size());

    transform(mMapperFunctions.begin(), mMapperFunctions.end(),
              mapperFunctionNames.begin(), [](auto mapperPtr) -> std::string {
                return mapperPtr->getName();
            });

    return mapperFunctionNames;
}

// When user selects a name (string), use this function to get the actual ptr.
std::shared_ptr<MapperFunctionT> FocusFinderLogicT::getMapperFunctionByName(
        const std::string &mapperFunctionName) const {

    auto it =
            std::find_if(mMapperFunctions.begin(), mMapperFunctions.end(),
                         [=](const std::shared_ptr<MapperFunctionT>& mapper) {
                             return mapper->getName() == mapperFunctionName;
                         });

    bool found = (it != mMapperFunctions.end());

    LOG(debug)
        << "Mapper '" << mapperFunctionName << " '"
        << (found ? "found" : "NOT found") << "..." << std::endl;

    return (found ? *it : nullptr);
}

std::shared_ptr<ImageConverter16To8T> FocusFinderLogicT::getImageConverter16To8() const {
    return mImageConverter16To8;
}

std::shared_ptr<TaskExecutorT<FocusFinderT> > FocusFinderLogicT::getFocusFinderExecutor() const {
    return mFocusFinderExecutor;
}
