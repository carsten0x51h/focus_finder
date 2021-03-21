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

#include <memory>
#include <optional>
#include <algorithm>

#include "include/focus_finder_logic.h"

#include "include/device_manager.h"
#include "include/device_manager_type.h"
#include "include/device_manager_factory.h"

#include "include/fofi_config_manager.h"
#include "include/global_config_manager.h"
#include "include/profile_manager.h"
#include "include/focus_finder_profile.h"
#include "include/logging.h"
#include "include/snr.h"
#include "include/centroid.h"
#include "include/thresholding_algorithm_factory.h"
#include "include/thresholding_algorithm.h"
#include "include/star_cluster_algorithm.h"

#include "include/linear_bw_stretch_mapper_function.h"
#include "include/spline_curve_mapper_function.h"
#include "include/image_converter_16to8.h"
#include "include/focus_finder.h"
#include "include/task_executor.h"
#include "include/focus_curve_recorder_logic.h"

FocusFinderLogicT *FocusFinderLogicT::sSelf = nullptr;

void FocusFinderLogicT::init() {
    sSelf = new FocusFinderLogicT;

    logging::trivial::severity_level sev = logging::trivial::debug;
    LoggingT::init(sev, true /*console*/, true /*log file*/);
}

void FocusFinderLogicT::close() {
    delete sSelf;
    sSelf = 0;
}

FocusFinderLogicT *FocusFinderLogicT::get() {
    return sSelf;
}

FocusFinderLogicT::FocusFinderLogicT() :
        mCameraDevice(nullptr) {
    LOG(debug)
        << "FocusFinderLogicT::FocusFinderLogicT()..." << std::endl;


    mFoFiConfigManager = std::make_shared<FoFiConfigManagerT>();



    // Create the device manager
    //mDeviceManager = std::make_shared<DummyDeviceManagerT>();

    // Initialize the INDI DeviceManager
    //
    // TODO: Question: There is just a generic interface to the DeviceManager here.
    //       Hence, there is no special method to set any INDI specific props.
    //       However, the IndiDeviceManagerT will have additional methods.
    //       Where will those methods be called?
    //       -> Maybe right in this class. There could be #ifdef statements to distinguish
    //          between the different OSs. There the one or the other DeviceManager could be
    //          instantiated and also configured.
    //
    // TODO: Port and hostname should not be hardcoded...
    //

    // IDEA: Maybe just pass the DeviceManager from outside... because
    //       which devicwe manager we use here does not matter. The configuration
    //       of the device manager should happen outside this class and the required
    //       parameters may also vary. They may come from a CMD lin app or from aGUI
    //       or from a web-app or whatever...



    // TODO: Allow e.g. a CMD line program to pass in the device manager type as cmd line param?  -> yes, then a GlobalFofiConfig object will just be manipulated (as part of mFoFiConfigManager...)
    // But how to handle device manager specific params - e.g. INDI params? Will there also be an IndiGlobalConfigT object?? - just as a generic object list / map? Or maybe .. static_cast<>
    // from a gneric DeviceManagerConfigT to an IndiDeviceManagerConfigT inside IndiDeviceManager?? This object may be created from the config file and may also be overwritten by cmdline params...
    // Since this overriding only can take place as part of the CMD line app, the rsepective config object must not be generated in here!
    //TODO...

    DeviceManagerTypeT::TypeE deviceManagerType = mFoFiConfigManager->getGlobalConfigManager()->getConfig().getDeviceManagerType();

    LOG(debug) << "DeviceManagerType configured: " << DeviceManagerTypeT::asStr(deviceManagerType) << std::endl;

    mDeviceManager = DeviceManagerFactoryT::getInstance(deviceManagerType);
    //mDeviceManager->configure(fullFilePath.....???);


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
            std::bind(&FocusFinderLogicT::updateProfile, this));


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

FocusFinderLogicT::~FocusFinderLogicT() {
}

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
                    // TODO: "mLastFrame" needs mutex guard!!!-> or atomic?
                    mLastFrame.setImage(resultImage);  // null if no image
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

size_t FocusFinderLogicT::calcNumStarsInRegion(const ImageT &inImg) const {

    auto thAlgo = ThresholdingAlgorithmFactoryT::getInstance(ThresholdingAlgorithmTypeT::MAX_ENTROPY);
    float th = thAlgo->calc(inImg, 16 /*bit depth - TODO: Do not hardcode - but where to query??*/);
    int thUp = std::ceil(th);

    LOG(debug) << "FocusFinderLogicT::calcNumStarsInRegion - threshold: " << th << ", thUp: " << thUp << std::endl;

    ImageT binaryImg = inImg.get_threshold(thUp + 1 /*HACK!*/); // Threshold function somehow uses >=

    StarClusterAlgorithmT starClusterAlgorithm(
            2 /*defines the allowed number of dark pixels between two white pixels until they form a cluster*/);
    std::list<StarClusterT> clusters = starClusterAlgorithm.cluster(binaryImg);

    LOG(debug) << "FocusFinderLogicT::calcNumStarsInRegion - Found " << clusters.size() << " stars..." << std::endl;

    return clusters.size();
}

// TODO: PointT<float> & poi -> PointT<int> & poi ??
std::optional<PointT<float> > FocusFinderLogicT::findFocusStar(
        const PointT<float> &poiFloat) {

    PointT<int> poi = poiFloat.to<int>();

    auto activeProfile = mFoFiConfigManager->getProfileManager()->getActiveProfile();

    if (!activeProfile) {
        LOG(warning) << "No active profile set!" << std::endl;
        return std::nullopt;
    }

    // TODO: Need mutex guard for 'mLastFrame'!!!-> or atomic?
    auto img = mLastFrame.getImage();

    if (img == nullptr) {
        LOG(warning)
            << "FocusFinderLogicT::findFocusStar... failed - no frame available."
            << std::endl;
        return std::nullopt;
    }

    // NOTE: Point "poi" passed in is in "pixmap" coordinates, i.e. in case there is
    //       just a subframe image available, the coordinates need to be transformed.
    //       In case roi is not set, roiX and roiY are just 0. That means we can make
    //       the same transformation always - just in case of a "full frame" it simply
    //       has no effect.
    auto frameBounds = mLastFrame.getBounds().to<int>(); // RectT<unsigned int>

    // TODO / HACK: We need a RectT type conversion - see QRect and QRectF - they have the same problem.
//	auto frameBounds = RectT<float>(frameBoundsI.x(), frameBoundsI.y(),
//			frameBoundsI.width(), frameBoundsI.height());
//	auto frameBounds = RectT<int>(frameBoundsI.x(), frameBoundsI.y(),
//			frameBoundsI.width(), frameBoundsI.height());

    // TODO: Coordinates are not 100% correct!!?? Needs debug / Check...

    // Extract "searchWindow" from seachCenter point.
    SizeT<unsigned int> starWindowSize = activeProfile->getStarWindowSize();

//	RectT<float> searchWindowRect = RectT<float>::fromCenterPoint(poi,
//			starWindowSize.width(), starWindowSize.height());
    RectT<int> searchWindowRect = RectT<int>::fromCenterPoint(poi, starWindowSize.to<int>());

    LOG(debug) << "FocusFinderLogicT::findFocusStar - searchWindowRect: "
               << " x=" << searchWindowRect.x() << ", y=" << searchWindowRect.y()
               << ", w=" << searchWindowRect.width() << ", h=" << searchWindowRect.height()
               << std::endl;


    // Handle case that user clicked to close to the boundary (see OLD code).
    bool selectionInsideBounds = frameBounds.contains(searchWindowRect);


    if (!selectionInsideBounds) {
        LOG(warning)
            << "FocusFinderLogicT::findFocusStar... selection window out of bounds."
            << std::endl;
        return std::nullopt;
    }

    // Star finder expects window coordinates in the coordinate system of the given image (ROI).
    // In case a ROI is set, "img" only is the sub-image - i.e. left-upper corner of the image
    // coordinate starts with (0,0). Therefore "searchWindowRect" needs o be transformed to those
    // "ROI" coordinates.
    //
    //
    // NOTE: CImg doc - boundary_conditions = Can be { 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }.
    // 		 In mathematics, the Dirichlet (or first-type) boundary condition is a type of boundary condition,
    // 		 named after Peter Gustav Lejeune Dirichlet (1805–1859).
    //
    //		 See: https://en.wikipedia.org/wiki/Dirichlet_boundary_condition
    //
    //
    // 		 In mathematics, the Neumann (or second-type) boundary condition is a type of boundary condition,
    //		 named after Carl Neumann.[1] When imposed on an ordinary or a partial differential equation, the
    //		 condition specifies the values in which the derivative of a solution is applied within the boundary
    //       of the domain.
    //
    // 		 See https://en.wikipedia.org/wiki/Neumann_boundary_condition
    ImageT searchWindowImg = img->get_crop(
            searchWindowRect.x() - frameBounds.x() /*x0*/,
            searchWindowRect.y() - frameBounds.y() /*y0*/,
            searchWindowRect.x() + searchWindowRect.height()
            - frameBounds.x() - 1 /*x1*/,
            searchWindowRect.y() + searchWindowRect.width()
            - frameBounds.y() - 1 /*y1*/);

    // Calculate the SNR to check if there COULD be a star around.
    float snr = SnrT::calculate(searchWindowImg);
    LOG(debug)
        << "SNR: " << snr << std::endl;

    if (snr < activeProfile->getStarDetectionSnrBoundary()) {
        // The 1 is just a rough guess at the moment...
        LOG(warning)
            << "FocusFinderLogicT::findFocusStar... no valid star detected."
            << std::endl;
        return std::nullopt;
    }


    // Check that there is only a single star in the selected region
    size_t numStarsInRegion = calcNumStarsInRegion(searchWindowImg);

    if (numStarsInRegion != 1) {
        LOG(warning)
            << "FocusFinderLogicT::findFocusStar... none or multiple stars in region detected."
            << std::endl;
        return std::nullopt;
    }


    // Try to calculate the centroid of the star image
    //
    // TODO: In addition we may want to configure
    //		 -Centroid type
    //		 -subtract mean (?) - should this be configured??
    //       -starRect = size of the star rect / "window" (from profile cfg)
    //       -maxSearchRect = 2 or 3 * starRect - max search boundary (from profile cfg)
    ImageT calcImage;
    auto calcCenterOpt = CentroidT::calculate(searchWindowImg,
                                              CentroidTypeT::IWC, true /*sub mean*/, &calcImage);

    // If there is a result, transform the returned coordinates (sub frame system)
    // back to the "window" coordinates (directly replace in the same optinal object.
    if (calcCenterOpt) {
        calcCenterOpt.emplace(
                PointT<float>(calcCenterOpt->x() + searchWindowRect.x(),
                              calcCenterOpt->y() + searchWindowRect.y()));
    }

    // TODO: Is it a good idea to set it here?
    mLastFocusStarPos = calcCenterOpt;

    return calcCenterOpt;
}

std::optional<PointT<float> > FocusFinderLogicT::getLastFocusStarPos() const {
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
                         [=](const std::shared_ptr<MapperFunctionT> mapper) {
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
