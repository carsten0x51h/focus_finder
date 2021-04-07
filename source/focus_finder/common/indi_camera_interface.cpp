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

// NOTE: INDI notifies also if the status of a vecProp has changed!!
#include <thread>
#include <chrono>
#include <memory>
#include <functional>
#include <list>
#include <unistd.h>
#include <filesystem>

#include "include/cimg_fits_io.h"
#include "include/reporting.h"
#include "include/indi_helper.h"
#include "include/indi_camera_interface.h"

#include "basedevice.h"

IndiCameraInterfaceT::IndiCameraInterfaceT(IndiDeviceT *indiDevice) : mIndiDevice(indiDevice),
                                                                      cancelExposureFlag(false),
                                                                      mIsExposureRunning(false), mLoopMode(
                LoopModeT::SINGLE), mExposureDelay(0s), mExposureTime(1s) {

    LOG(debug) << "IndiCameraInterfaceT::IndiCameraInterfaceT..." << std::endl;

    IndiClientT *indiClient = mIndiDevice->getIndiClient();

    //mIndiConnector = std::make_shared < IndiDeviceT > (dp, indiClient);

    //TODO: mIndiConnector->setUsbDevicePort();

    // Register number
    mNewNumberConnection = indiClient->registerNewNumberListener(
            [this](INumberVectorProperty *nvp) { newNumber(nvp); }
    );

    // Register on image recipient event
    mNewBlobConnection = indiClient->registerNewBlobListener(
            [this](IBLOB *blob) { newBlob(blob); }
    );

    mIndiDevice->getIndiClient()->setBLOBMode(B_ALSO, mIndiDevice->getIndiBaseDevice()->getDeviceName(), nullptr);

    // Register switch
    mNewSwitchConnection = indiClient->registerNewSwitchListener(
            [this](ISwitchVectorProperty *svp) { newSwitch(svp); }
    );
}

IndiCameraInterfaceT::~IndiCameraInterfaceT() {
    LOG(debug) << "IndiCameraInterfaceT::IndiCameraInterfaceTnterfaceT..." << std::endl;

    IndiClientT *indiClient = mIndiDevice->getIndiClient();

    indiClient->unregisterNewNumberListener(mNewNumberConnection);
    indiClient->unregisterNewBlobListener(mNewBlobConnection);
    indiClient->unregisterNewSwitchListener(mNewSwitchConnection);
}

DeviceT *IndiCameraInterfaceT::getParentDevice() {
    return mIndiDevice;
}

/**
 *  Throws if something goes wrong...
 *
 *  See http://cboard.cprogramming.com/c-programming/82961-example-mkstemp.html
 */
std::shared_ptr<ImageT> IndiCameraInterfaceT::convertIndiBlobToCImg(IBLOB *iblob) const {
    const static size_t MAX_TMP_FILENAME_SIZE = 15;

    char sfn[MAX_TMP_FILENAME_SIZE] = "";
    FILE *sfp;
    int fd;

    strncpy(sfn, "/tmp/ed.XXXXXX", sizeof sfn);

    if ((fd = mkstemp(sfn)) == -1 || (sfp = fdopen(fd, "w+")) == nullptr) {

        if (fd != -1) {
            unlink(sfn);
            close(fd);
        }

        LOG(fatal) << "Unable to create temporary FITS file '" << sfn
                   << "', error='" << strerror(errno) << "'" << std::endl;

        // TODO: throw exc?!

    } else {
        LOG(debug) << "Successfully opened TMP file. Writing image to "
                   << sfn << std::endl;

        // TODO: IDEA - we may do the write below in chunks to have a better
        //       indication of the porgress.
        notifyFrameTransferUpd(0);

        // TODO: Replace write with filesystem::write...
        size_t bytesWritten = fwrite(static_cast<char *>(iblob->blob),
                                     sizeof(char), iblob->bloblen, sfp);

        LOG(debug) << "bytesWritten: " << bytesWritten << " to file: "
                   << sfn << std::endl;

        // TODO: Use filesystem::close...
        fclose(sfp);

        notifyFrameTransferUpd(50);

        // Read file
        std::shared_ptr<ImageT> resultImage = std::make_shared<ImageT>();

        LOG(debug)
            << "IndiCameraInterfaceT::convertIndiBlobToCImg... reading FITS file '"
            << sfn << "'" << std::endl;

        try {
            std::stringstream ss;
            long bitsPerPixel;
            CImgFitsIOHelperT::readFits(resultImage.get(), sfn, &bitsPerPixel,
                                        &ss);

            LOG(debug)
                << "IndiCameraInterfaceT::convertIndiBlobToCImg... FITS file '"
                << sfn << "' read, bitsPerPixel=" << bitsPerPixel
                << ", details: " << ss.str() << std::endl;

        } catch (FitsIOExceptionT &exc) {
            LOG(fatal) << "Reading FITS file failed: " << exc.what()
                       << std::endl;

            return nullptr;
        }

        notifyFrameTransferUpd(100);

        // Removing tmp file
        std::error_code ec;
        std::filesystem::remove(sfn, ec);

        if (ec) {
            LOG(warning) << "Unable to delete temporary FITS file '" << sfn
                         << "', error='" << ec.message() << "'." << std::endl;
        }

        return resultImage;
    }
    return nullptr;
}

void IndiCameraInterfaceT::newNumber(INumberVectorProperty *nvp) {
    if (strcmp(nvp->device, getName().c_str()) == 0) {

        LOG(debug) << "IndiCameraInterfaceT::newNumber... " << nvp->name
                   << " has changed..." << std::endl;

        // TODO: Handle ? -> Are we interested in any number update? (especially status changes?!!!)

        // NOTE: Call of notify is limited to state IPS_BUSY since otherwise called again
        //       after notification of "exposure finished" was already called.
        if (strcmp(nvp->name, "CCD_EXPOSURE") == 0) {

            if (nvp->s == IPS_BUSY) {
                INumber *exposureTimeNum = IndiHelperT::getNumber(nvp,
                                                                  "CCD_EXPOSURE_VALUE");

                notifyExposureTimerUpd(exposureTimeNum->value);
            } else if (nvp->s == IPS_ALERT) {

                // Report problem... -TODO: good idea here? from sep. thread...
                ReportingT::reportMsg(ReportingDatasetT("IndiCamera", "Exposure failed. Cancelling exposure.",
                                                        "CCD_EXPOSURE property reported IPS_ALERT."));

                // Cancel exposure
                // In any case set cancel flag - TODO: Is this a good diea?
                cancelExposureFlag = true;

                cv.notify_all();
            }
        }
    }
}

void IndiCameraInterfaceT::newBlob(IBLOB *blob) {
    IBLOBVectorProperty *blobVec = blob->bvp;

    if (strcmp(blobVec->device, getName().c_str()) == 0) {
        LOG(debug) << "IndiCameraInterfaceT::newBlob...name=" << blob->name
                   << ", blob=" << blob << std::endl;

        // Check if blob is really an "image"
        if (strcmp(blob->name, "CCD1") == 0 && blobVec->s == IPS_OK) {

            LOG(debug) << "blobVec - device: " << blobVec->device
                       << ", name: " << blobVec->name << ", label: "
                       << blobVec->label << ", group: " << blobVec->group
                       << ", p: " << blobVec->p << ", timeout: "
                       << blobVec->timeout << ", s: " << blobVec->s << ", bp: "
                       << blobVec->bp << ", nbp: " << blobVec->nbp << ", aux: "
                       << blobVec->aux << std::endl;

            mResultImage = convertIndiBlobToCImg(blob);

            LOG(debug)
                << "IndiCameraInterfaceT::newBlob... image stored, now notify_all()..."
                << std::endl;

            cv.notify_all();
        }
    }
}

void IndiCameraInterfaceT::newSwitch(ISwitchVectorProperty *svp) {
    if (strcmp(svp->device, getName().c_str()) == 0) {

        LOG(debug) << "IndiCameraInterfaceT::newSwitch... " << svp->name
                   << " has changed..." << std::endl;

        if (strcmp(svp->name, "CCD_ABORT_EXPOSURE") == 0) {

            ISwitch *abortSwitch = IndiHelperT::getSwitch(svp, "ABORT");

            if (svp->s == IPS_OK) {
                bool exposureCancelled = (abortSwitch->s == ISS_OFF);

                LOG(info) << "Exposure cancelled: " << exposureCancelled << ", status: "
                          << IndiHelperT::propStateAsStr(svp->s) << std::endl;

                cancelExposureFlag = true;
                cv.notify_all();
            } else {
                // Report problem...
                ReportingT::reportMsg(ReportingDatasetT("IndiCamera", "Aborting exposure failed.", "" /*no details*/));
            }
        }
    }
}

/////////////////////////////////////////////////
// Device
/////////////////////////////////////////////////
std::string IndiCameraInterfaceT::getName() const {
    return mIndiDevice->getIndiBaseDevice()->getDeviceName();
}

// Hardware properties
unsigned int IndiCameraInterfaceT::getBitsPerPixel() const {

    try {
        INumberVectorProperty *ccdInfoVecProp = IndiHelperT::getNumberVec(
                mIndiDevice->getIndiBaseDevice(), "CCD_INFO");

        IndiHelperT::requireReadable(ccdInfoVecProp);

        INumber *bpp = IndiHelperT::getNumber(ccdInfoVecProp,
                                              "CCD_BITSPERPIXEL");

        return (unsigned int) bpp->value;
    } catch (IndiExceptionT &exc) {
        throw CameraExceptionT(
                "Unable to get bits per pixel. Details: "
                + std::string(exc.what()));
    }
}

SizeT<float> IndiCameraInterfaceT::getPixelSize() const {
    try {
        INumberVectorProperty *ccdInfoVecProp = IndiHelperT::getNumberVec(
                mIndiDevice->getIndiBaseDevice(), "CCD_INFO");

        IndiHelperT::requireReadable(ccdInfoVecProp);

        // First try CCD_PIXEL_SIZE_X and CCD_PIXEL_SIZE_Y
        INumber *pixelSizeX = IndiHelperT::getNumber(ccdInfoVecProp,
                                                     "CCD_PIXEL_SIZE_X");
        INumber *pixelSizeY = IndiHelperT::getNumber(ccdInfoVecProp,
                                                     "CCD_PIXEL_SIZE_Y");

        return SizeT<float>(pixelSizeX->value, pixelSizeY->value);

    } catch (IndiExceptionT &exc) {
        throw CameraExceptionT(
                "Unable to get pixel size. Details: "
                + std::string(exc.what()));
    }
}

SizeT<unsigned int> IndiCameraInterfaceT::getMaxResolution() const {

    try {
        INumberVectorProperty *ccdInfoVecProp = IndiHelperT::getNumberVec(
                mIndiDevice->getIndiBaseDevice(), "CCD_INFO");

        IndiHelperT::requireReadable(ccdInfoVecProp);

        INumber *maxX = IndiHelperT::getNumber(ccdInfoVecProp, "CCD_MAX_X");
        INumber *maxY = IndiHelperT::getNumber(ccdInfoVecProp, "CCD_MAX_Y");

        return SizeT<unsigned int>(maxX->value, maxY->value);

    } catch (IndiExceptionT &exc) {
        throw CameraExceptionT(
                "Unable to get max resolution. Details: "
                + std::string(exc.what()));
    }
}

std::chrono::milliseconds IndiCameraInterfaceT::getMinExposureTime() const {

    try {
        INumberVectorProperty *exposureTimeVecProp = IndiHelperT::getNumberVec(
                mIndiDevice->getIndiBaseDevice(), "CCD_EXPOSURE");

        IndiHelperT::requireReadable(exposureTimeVecProp);

        INumber *exposureTime = IndiHelperT::getNumber(exposureTimeVecProp,
                                                       "CCD_EXPOSURE_VALUE");

        // See: https://stackoverflow.com/questions/14391327/how-to-get-duration-as-int-millis-and-float-seconds-from-chrono
        std::chrono::duration<float> fs(exposureTime->min);

        std::chrono::milliseconds d = std::chrono::duration_cast
                <std::chrono::milliseconds>(fs);

        LOG(debug) << "IndiCameraInterfaceT::getMinExposureTime - min: "
                   << exposureTime->min << ", sec: " << fs.count() << ", ms: " << d.count() << std::endl;

        return d;

    } catch (IndiExceptionT &exc) {
        throw CameraExceptionT(
                "Unable to get min exposure time. Details: "
                + std::string(exc.what()));
    }
}

std::chrono::milliseconds IndiCameraInterfaceT::getMaxExposureTime() const {
    try {
        INumberVectorProperty *exposureTimeVecProp = IndiHelperT::getNumberVec(
                mIndiDevice->getIndiBaseDevice(), "CCD_EXPOSURE");

        IndiHelperT::requireReadable(exposureTimeVecProp);

        INumber *exposureTime = IndiHelperT::getNumber(exposureTimeVecProp,
                                                       "CCD_EXPOSURE_VALUE");

        // See: https://stackoverflow.com/questions/14391327/how-to-get-duration-as-int-millis-and-float-seconds-from-chrono
        std::chrono::duration<float> fs(exposureTime->max);

        std::chrono::milliseconds d = std::chrono::duration_cast
                <std::chrono::milliseconds>(fs);

        LOG(debug) << "IndiCameraInterfaceT::getMaxExposureTime - max: "
                   << exposureTime->min << ", sec: " << fs.count() << ", ms: "
                   << d.count() << std::endl;

        return d;

    } catch (IndiExceptionT &exc) {
        throw CameraExceptionT(
                "Unable to get max exposure time. Details: "
                + std::string(exc.what()));
    }
}

// Camera
// Exposure related properties
void IndiCameraInterfaceT::startExposure() {

    // TODO / BUG: When "Loop" is enabled, an "Exposure already running" exception is thrown...
    if (DeviceConnectionStateT::CONNECTED
        != mIndiDevice->getConnectionState()) {
        // TODO: Maybe better reporting? + return
        throw CameraExceptionT("Camera not connected.");
    }

    if (isExposureRunning()) {
        // TODO: Maybe better reporting? + return
        throw CameraExceptionT("Exposure already running.");
    }
    cancelExposureFlag = false;

    exposureThread = std::thread(&IndiCameraInterfaceT::expose, this);
    exposureThread.detach();
}

void IndiCameraInterfaceT::cancelExposure() {
    LOG(debug) << "IndiCameraInterfaceT - cancelExposure..." << std::endl;

    // TODO: Introduce and check if: isExposureAbortSupported()...

    try {
        // Tell camera to cancel...
        ISwitchVectorProperty *abortExposureVecProp =
                IndiHelperT::getSwitchVec(mIndiDevice->getIndiBaseDevice(),
                                          "CCD_ABORT_EXPOSURE");

        IndiHelperT::requireWritable(abortExposureVecProp);

        ISwitch *abortExposure = IndiHelperT::getSwitch(abortExposureVecProp,
                                                        "ABORT");
        abortExposure->s = ISS_ON;

        mIndiDevice->getIndiClient()->sendNewSwitch(abortExposureVecProp);

        // --> generates a newSwitch() event!

    } catch (IndiExceptionT &exc) {
        // Report problem...
        ReportingT::reportMsg(ReportingDatasetT("IndiCamera", "Aborting exposure failed.", exc.what()));

        // TODO!!!!!: Should this class actually use Reporting:: ? Or should it throw a CameraExceptionT and we catch it there and call ReportingT:: there..?!
    }

    // In any case set cancel flag - TODO: Is this a good idea?
//	cancelExposureFlag = true;
//
//	cv.notify_all();
}

/**
 * NOTE: Checking the property CCD_EXPOSURE is not reliable because
 * of a delay between setting the property here and its effect on
 * the server side. Therefore, an internal flag "mIsExposureRunning"
 * is used to track the exposure status.
 */
bool IndiCameraInterfaceT::isExposureRunning() {
    return mIsExposureRunning.load();
}

std::chrono::milliseconds IndiCameraInterfaceT::getExposureTime() const {
    return mExposureTime;
}

void IndiCameraInterfaceT::setExposureTime(
        const std::chrono::milliseconds &exposureTime) {
    mExposureTime = exposureTime;
}

void IndiCameraInterfaceT::setExposureDelay(
        const std::chrono::milliseconds &exposureDelay) {
    mExposureDelay = exposureDelay;
}

std::chrono::milliseconds IndiCameraInterfaceT::getExposureDelay() const {
    return mExposureDelay;
}

LoopModeT::TypeE IndiCameraInterfaceT::getLoopMode() const {
    return mLoopMode.load();
}

void IndiCameraInterfaceT::setLoopMode(LoopModeT::TypeE loopMode) {
    mLoopMode = loopMode;
}

std::list<FrameTypeT::TypeE> IndiCameraInterfaceT::getSupportedFrameTypes() const {
    LOG(warning) << "IndiCameraInterfaceT::getSupportedFrameTypes() NOT YET IMPLEMENTED." << std::endl;

    return {FrameTypeT::LIGHT, FrameTypeT::BIAS, FrameTypeT::DARK, FrameTypeT::FLAT};
}

FrameTypeT::TypeE IndiCameraInterfaceT::getFrameType() const {
    LOG(warning) << "IndiCameraInterfaceT::getFrameType() NOT YET IMPLEMENTED." << std::endl;

    return FrameTypeT::LIGHT;
}

void IndiCameraInterfaceT::setFrameType(FrameTypeT::TypeE /*frameType*/) {
    // TODO: Implement
    LOG(warning) << "IndiCameraInterfaceT::setFrameType() NOT YET IMPLEMENTED." << std::endl;
}

BinningT IndiCameraInterfaceT::getSupportedMaxBinningMode() const {
    // TODO: Implement
    LOG(warning) << "IndiCameraInterfaceT::getSupportedMaxBinningMode() NOT YET IMPLEMENTED." << std::endl;

    return BinningT(5, 5);
}

BinningT IndiCameraInterfaceT::getBinning() const {
    // TODO: Implement
    LOG(warning) << "IndiCameraInterfaceT::getBinning() NOT YET IMPLEMENTED." << std::endl;

    return BinningT(1, 1);
}

void IndiCameraInterfaceT::setBinning(const BinningT & /*binning*/) {
    // TODO...
    LOG(warning) << "IndiCameraInterfaceT::setBinning() NOT YET IMPLEMENTED." << std::endl;
}

bool IndiCameraInterfaceT::isRoiSupported() const {
    LOG(warning) << "IndiCameraInterfaceT::isRoiSupported() NOT YET IMPLEMENTED." << std::endl;

    return true;
}

RectT<unsigned int> IndiCameraInterfaceT::getRoi() const {
    return mSelectedRoi;
}

void IndiCameraInterfaceT::setRoi(const RectT<unsigned int> &selectedSubFrame) {
    LOG(debug) << "IndiCameraInterfaceT::setSubFrame... subFrame: "
               << selectedSubFrame.x() << ", " << selectedSubFrame.y() << ", w: "
               << selectedSubFrame.width() << ", h: " << selectedSubFrame.height()
               << std::endl;

    mSelectedRoi = selectedSubFrame;
}

void IndiCameraInterfaceT::clearRoi() {
    mSelectedRoi.clear();
}

bool IndiCameraInterfaceT::delayedExposueTimer() {

    int exosureDelayMillis = mExposureDelay.count();
    bool result = true;

    notifyExposureDelayTimerUpd(exosureDelayMillis);

    while (exosureDelayMillis > 0) {
        LOG(debug) << "Next exposure starts in '" << exosureDelayMillis
                   << "ms'..." << std::endl;

        if (cancelExposureFlag.load()) {
            result = false;
            break;
        }

        // During loop notify listeners only every 500 ms
        if (exosureDelayMillis % 500 == 0) {
            notifyExposureDelayTimerUpd(exosureDelayMillis);
        }

        std::this_thread::sleep_for(1ms);
        --exosureDelayMillis;
    }

    notifyExposureDelayTimerUpd(exosureDelayMillis);

    return result;
}

void IndiCameraInterfaceT::setRoiIndi(const RectT<unsigned int> &roi) {

    INumberVectorProperty *frameVecProp = IndiHelperT::getNumberVec(
            mIndiDevice->getIndiBaseDevice(), "CCD_FRAME");

    IndiHelperT::requireWritable(frameVecProp);

    INumber *pNumX = IndiHelperT::getNumber(frameVecProp, "X");
    INumber *pNumY = IndiHelperT::getNumber(frameVecProp, "Y");
    INumber *pNumWidth = IndiHelperT::getNumber(frameVecProp, "WIDTH");
    INumber *pNumHeight = IndiHelperT::getNumber(frameVecProp, "HEIGHT");

    if (roi.isSet()) {
        pNumX->value = roi.x();
        pNumY->value = roi.y();
        pNumWidth->value = roi.width();
        pNumHeight->value = roi.height();
    } else {
        SizeT<unsigned int> maxRes = getMaxResolution();

        pNumX->value = 0;
        pNumY->value = 0;
        pNumWidth->value = maxRes.width();
        pNumHeight->value = maxRes.height();
    }

    mIndiDevice->getIndiClient()->sendNewNumber(frameVecProp);
}

bool IndiCameraInterfaceT::indiExposure() {
    LOG(debug) << "IndiCameraInterfaceT - startExposure..." << std::endl;

    // TODO: Also set binning, frame type, etc...??

    // Set ROI
    try {

        setRoiIndi(mSelectedRoi);

    } catch (IndiExceptionT &exc) {
        ReportingT::reportMsg(ReportingDatasetT("IndiCamera", "Aborting exposure failed.", exc.what()));

        return false;
    }

    // Set exposure time (-> starts exposure)
    try {

        INumberVectorProperty *exposureTimeVecProp = IndiHelperT::getNumberVec(
                mIndiDevice->getIndiBaseDevice(), "CCD_EXPOSURE");

        IndiHelperT::requireWritable(exposureTimeVecProp);

        INumber *exposureTimeNum = IndiHelperT::getNumber(exposureTimeVecProp,
                                                          "CCD_EXPOSURE_VALUE");

        auto seconds = std::chrono::duration_cast<std::chrono::duration<float>
        >(mExposureTime);

        exposureTimeNum->value = seconds.count(); // In seconds

        mIndiDevice->getIndiClient()->sendNewNumber(exposureTimeVecProp);

    } catch (IndiExceptionT &exc) {
        ReportingT::reportMsg(ReportingDatasetT("IndiCamera", "Unable to set exposure time.", exc.what()));

        return false;
    }

    // Wait until exposure finished or cancelled
    LOG(debug) << "Waiting for exposure to finish or cancel..."
               << std::endl;

    std::unique_lock<std::mutex> lk(cvMutex);

    cv.wait(lk);

    return (!cancelExposureFlag.load());
}

void IndiCameraInterfaceT::expose() {
    bool cancelled;
    bool lastExposure;

    do {
        mResultImage = std::make_shared<ImageT>();

        mIsExposureRunning = true;

        // https://stackoverflow.com/questions/7925479/if-argument-evaluation-order
        // https://stackoverflow.com/questions/18450585/what-is-the-order-of-evaluation-of-statements-in-a-if-bracket-if
        cancelled = !delayedExposueTimer() || !indiExposure();

        lastExposure = (getLoopMode() != LoopModeT::LOOP);

        mIsExposureRunning = false;

        if (cancelled) {
            LOG(debug) << "Exposure cancelled." << std::endl;
            notifyExposureCancelled();
        } else {
            LOG(debug) << "Exposure finished." << std::endl;
            notifyExposureCycleFinished(mSelectedRoi, mResultImage,
                                        lastExposure);
        }

    } while (!lastExposure && !cancelled);
}
