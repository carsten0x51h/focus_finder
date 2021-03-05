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
#include <atomic>
#include <memory>
#include <functional>
#include <utility>
#include <list>
#include <unistd.h>
#include <filesystem>

#include "include/cimg_fits_io.h"
#include "include/reporting.h"
#include "include/indi_helper.h"
#include "include/indi_camera.h"

#include "basedevice.h"

IndiCameraT::IndiCameraT(INDI::BaseDevice *dp, IndiClientT * indiClient) :
		mIndiBaseDevice(dp), mIndiClient(indiClient), cancelExposureFlag(false), mIsExposureRunning(false), mLoopMode(
				LoopModeT::SINGLE), mExposureDelay(0s), mExposureTime(1s) {
  
	LOG(debug) << "IndiCameraT::IndiCameraT..." << std::endl;

	mDeviceType = DeviceTypeT::CAMERA;

	mIndiConnector = std::make_shared < IndiUsbDeviceConnectorT > (dp, indiClient);

	//TODO: mIndiConnector->setUsbDevicePort();
	
	// Register number
	mNewNumberConnection = mIndiClient->registerNewNumberListener(
			boost::bind(&IndiCameraT::newNumber, this, boost::placeholders::_1));

	// Register on image recipient event
  	mNewBlobConnection = mIndiClient->registerNewBlobListener(
       		boost::bind(&IndiCameraT::newBlob, this, boost::placeholders::_1));

    mIndiClient->setBLOBMode(B_ALSO, mIndiBaseDevice->getDeviceName(), nullptr);

    // Register switch
    mNewSwitchConnection = mIndiClient->registerNewSwitchListener(
          boost::bind(&IndiCameraT::newSwitch, this, boost::placeholders::_1));
}

IndiCameraT::~IndiCameraT() {
	LOG(debug) << "IndiCameraT::~IndiCameraT..." << std::endl;

	mIndiClient->unregisterNewNumberListener(mNewNumberConnection);
	mIndiClient->unregisterNewBlobListener(mNewBlobConnection);
    mIndiClient->unregisterNewSwitchListener(mNewSwitchConnection);
}

/**
 *  Throws if something goes wrong...
 *
 *  See http://cboard.cprogramming.com/c-programming/82961-example-mkstemp.html
 */
std::shared_ptr<ImageT> IndiCameraT::convertIndiBlobToCImg(IBLOB* iblob) const {
	char sfn[15] = "";
	FILE * sfp;
	int fd = -1;

	strncpy(sfn, "/tmp/ed.XXXXXX", sizeof sfn);

	if ((fd = mkstemp(sfn)) == -1 || (sfp = fdopen(fd, "w+")) == NULL) {

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
				<< "IndiCameraT::convertIndiBlobToCImg... reading FITS file '"
				<< sfn << "'" << std::endl;

		try {
			std::stringstream ss;
			long bitsPerPixel;
			CImgFitsIOHelperT::readFits(resultImage.get(), sfn, &bitsPerPixel,
					&ss);

			LOG(debug)
					<< "IndiCameraT::convertIndiBlobToCImg... FITS file '"
					<< sfn << "' read, bitsPerPixel=" << bitsPerPixel
					<< ", details: " << ss.str() << std::endl;

		} catch (FitsIOExceptionT & exc) {
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

void IndiCameraT::newNumber(INumberVectorProperty * nvp) {
	if (strcmp(nvp->device, getName().c_str()) == 0) {

		LOG(debug) << "IndiCameraT::newNumber... " << nvp->name
				<< " has changed..." << std::endl;

		// TODO: Handle ? -> Are we interested in any number update? (especially status changes?!!!)

		// NOTE: Call of notify is limited to state IPS_BUSY since otherwise called again
		//       after notification of "exposure finished" was already called.
		if (strcmp(nvp->name, "CCD_EXPOSURE") == 0) {

			if (nvp->s == IPS_BUSY) {
				INumber * exposureTimeNum = IndiHelperT::getNumber(nvp,
						"CCD_EXPOSURE_VALUE");

				notifyExposureTimerUpd(exposureTimeNum->value);
			} else if (nvp->s == IPS_ALERT) {

				// Report problem... -TODO: good idea here? from sep. thread...
				ReportingT::reportMsg(ReportingDatasetT("IndiCamera", "Exposure failed. Cancelling exposure.",
						"CCD_EXPOSURE property reported IPS_ALERT."));

				// Cancel exposure
				// In any case set cance flag - TODO: Is this a good diea?
				cancelExposureFlag = true;

				cv.notify_all();
			}
		}
	}
}

void IndiCameraT::newBlob(IBLOB* blob) {
	IBLOBVectorProperty * blobVec = blob->bvp;

	if (strcmp(blobVec->device, getName().c_str()) == 0) {
		LOG(debug) << "IndiCameraT::newBlob...name=" << blob->name
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
					<< "IndiCameraT::newBlob... image stored, now notify_all()..."
					<< std::endl;

			cv.notify_all();
		}
	}
}

void IndiCameraT::newSwitch(ISwitchVectorProperty* svp) {
    if (strcmp(svp->device, getName().c_str()) == 0) {

        LOG(debug) << "IndiCameraT::newSwitch... " << svp->name
                   << " has changed..." << std::endl;

        if (strcmp(svp->name, "CCD_ABORT_EXPOSURE") == 0) {

            ISwitch * abortSwitch = IndiHelperT::getSwitch(svp, "ABORT");

            if (svp->s == IPS_OK) {
                bool exposureCancelled = (abortSwitch->s == ISS_OFF);

                LOG(info) << "Exposure cancelled: " << exposureCancelled << ", status: " << IndiHelperT::propStateAsStr(svp->s) << std::endl;

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
std::string IndiCameraT::getName() const {
	return mIndiBaseDevice->getDeviceName();
}

std::shared_ptr<DeviceConnectorT> IndiCameraT::getConnector() const {
	return mIndiConnector;
}

// Hardware properties
unsigned int IndiCameraT::getBitsPerPixel() const {
	try {
		INumberVectorProperty * ccdInfoVecProp = IndiHelperT::getNumberVec(
				mIndiBaseDevice, "CCD_INFO");

		IndiHelperT::requireReadable(ccdInfoVecProp);

		INumber * bpp = IndiHelperT::getNumber(ccdInfoVecProp,
				"CCD_BITSPERPIXEL");

		return bpp->value;
	} catch (IndiExceptionT & exc) {
		throw CameraExceptionT(
				"Unable to get bits per pixel. Details: "
						+ std::string(exc.what()));
	}
}

SizeT<float> IndiCameraT::getPixelSize() const {
	try {
		INumberVectorProperty * ccdInfoVecProp = IndiHelperT::getNumberVec(
				mIndiBaseDevice, "CCD_INFO");

		IndiHelperT::requireReadable(ccdInfoVecProp);

		// First try CCD_PIXEL_SIZE_X and CCD_PIXEL_SIZE_Y
		INumber * pixelSizeX = IndiHelperT::getNumber(ccdInfoVecProp,
				"CCD_PIXEL_SIZE_X");
		INumber * pixelSizeY = IndiHelperT::getNumber(ccdInfoVecProp,
				"CCD_PIXEL_SIZE_Y");

		return SizeT<float>(pixelSizeX->value, pixelSizeY->value);

	} catch (IndiExceptionT & exc) {
		throw CameraExceptionT(
				"Unable to get pixel size. Details: "
						+ std::string(exc.what()));
	}
}

SizeT<unsigned int> IndiCameraT::getMaxResolution() const {

	try {
		INumberVectorProperty * ccdInfoVecProp = IndiHelperT::getNumberVec(
				mIndiBaseDevice, "CCD_INFO");

		IndiHelperT::requireReadable(ccdInfoVecProp);

		INumber * maxX = IndiHelperT::getNumber(ccdInfoVecProp, "CCD_MAX_X");
		INumber * maxY = IndiHelperT::getNumber(ccdInfoVecProp, "CCD_MAX_Y");

		return SizeT<unsigned int>(maxX->value, maxY->value);

	} catch (IndiExceptionT & exc) {
		throw CameraExceptionT(
				"Unable to get max resolution. Details: "
						+ std::string(exc.what()));
	}
}

std::chrono::milliseconds IndiCameraT::getMinExposureTime() const {

	try {
		INumberVectorProperty * exposureTimeVecProp = IndiHelperT::getNumberVec(
				mIndiBaseDevice, "CCD_EXPOSURE");

		IndiHelperT::requireReadable(exposureTimeVecProp);

		INumber * exposureTime = IndiHelperT::getNumber(exposureTimeVecProp,
				"CCD_EXPOSURE_VALUE");

		// See: https://stackoverflow.com/questions/14391327/how-to-get-duration-as-int-millis-and-float-seconds-from-chrono
		std::chrono::duration<float> fs(exposureTime->min);

		std::chrono::milliseconds d = std::chrono::duration_cast
				< std::chrono::milliseconds > (fs);

		LOG(debug) << "IndiCameraT::getMinExposureTime - min: "
				<< exposureTime->min << ", sec: " << fs.count() << ", ms: " << d.count() << std::endl;

		return d;

	} catch (IndiExceptionT & exc) {
		throw CameraExceptionT(
				"Unable to get min exposure time. Details: "
						+ std::string(exc.what()));
	}
}

std::chrono::milliseconds IndiCameraT::getMaxExposureTime() const {
	try {
		INumberVectorProperty * exposureTimeVecProp = IndiHelperT::getNumberVec(
				mIndiBaseDevice, "CCD_EXPOSURE");

		IndiHelperT::requireReadable(exposureTimeVecProp);

		INumber * exposureTime = IndiHelperT::getNumber(exposureTimeVecProp,
				"CCD_EXPOSURE_VALUE");

		// See: https://stackoverflow.com/questions/14391327/how-to-get-duration-as-int-millis-and-float-seconds-from-chrono
		std::chrono::duration<float> fs(exposureTime->max);

		std::chrono::milliseconds d = std::chrono::duration_cast
				< std::chrono::milliseconds > (fs);

		LOG(debug) << "IndiCameraT::getMaxExposureTime - max: "
				<< exposureTime->min << ", sec: " << fs.count() << ", ms: "
				<< d.count() << std::endl;

		return d;

	} catch (IndiExceptionT & exc) {
		throw CameraExceptionT(
				"Unable to get max exposure time. Details: "
						+ std::string(exc.what()));
	}
}

// Camera
// Exposure related properties
void IndiCameraT::startExposure() {

  // TODO / BUG: When "Loop" is enabled, an "Exposure already running" exception is thrown...
	if (DeviceConnectionStateT::CONNECTED
			!= mIndiConnector->getConnectionState()) {
		// TODO: Maybe better reporting? + return
		throw CameraExceptionT("Camera not connected.");
	}

	if (isExposureRunning()) {
		// TODO: Maybe better reporting? + return
		throw CameraExceptionT("Exposure already running.");
	}
	cancelExposureFlag = false;

	exposureThread = std::thread(&IndiCameraT::expose, this);
	exposureThread.detach();
}

void IndiCameraT::cancelExposure() {
	LOG(debug) << "IndiCameraT - cancelExposure..." << std::endl;

	// TODO: Introduce and check if: isExposureAbortSupported()...

	try {
		// Tell camera to cancel...
		ISwitchVectorProperty * abortExposureVecProp =
				IndiHelperT::getSwitchVec(mIndiBaseDevice,
						"CCD_ABORT_EXPOSURE");

		IndiHelperT::requireWritable(abortExposureVecProp);

		ISwitch * abortExposure = IndiHelperT::getSwitch(abortExposureVecProp,
				"ABORT");
		abortExposure->s = ISS_ON;

		mIndiClient->sendNewSwitch(abortExposureVecProp);

		// --> generates a newSwitch() event!

	} catch (IndiExceptionT & exc) {
		// Report problem...
		ReportingT::reportMsg(ReportingDatasetT("IndiCamera", "Aborting exposure failed.", exc.what()));

		// TODO!!!!!: Should this class actually use Reporting:: ? Or should it throw a CameraExceptionT and we catch it there and call ReportingT:: there..?!
	}

	// In any case set cancel flag - TODO: Is this a good idea?
//	cancelExposureFlag = true;
//
//	cv.notify_all();
}

// TODO: This is not always reliable because of a delay between setting the proerty and it taking the effect -> use own flag!
bool IndiCameraT::isExposureRunning() {

    return mIsExposureRunning.load();
//	bool exposureRunning  = false;
//
//	try {
//		INumberVectorProperty * exposureTimeVecProp = IndiHelperT::getNumberVec(
//				mIndiBaseDevice, "CCD_EXPOSURE");
//
//		exposureRunning = (exposureTimeVecProp->s == IPS_BUSY);
//
//	} catch (IndiExceptionT & exc) {
//		exposureRunning = false;
//	}
//
//	LOG(debug) << "IndiCameraT - isExposureRunning...exposureRunning=" << exposureRunning << std::endl;
//
//	return exposureRunning;
}

std::chrono::milliseconds IndiCameraT::getExposureTime() const {
	return mExposureTime;
}

void IndiCameraT::setExposureTime(
		const std::chrono::milliseconds & exposureTime) {
  mExposureTime = exposureTime;
  // TODO ! HACK!
  //using namespace std::chrono_literals;
  //mExposureTime = 1000ms;
}

void IndiCameraT::setExposureDelay(
		const std::chrono::milliseconds & exposureDelay) {
	mExposureDelay = exposureDelay;
}

std::chrono::milliseconds IndiCameraT::getExposureDelay() const {
	return mExposureDelay;
}

LoopModeT::TypeE IndiCameraT::getLoopMode() const {
	return mLoopMode.load();
}

void IndiCameraT::setLoopMode(LoopModeT::TypeE loopMode) {
	mLoopMode = loopMode;
}

std::list<FrameTypeT::TypeE> IndiCameraT::getSupportedFrameTypes() const {
	return {FrameTypeT::LIGHT, FrameTypeT::BIAS, FrameTypeT::DARK, FrameTypeT::FLAT};
}

FrameTypeT::TypeE IndiCameraT::getFrameType() const {
	return FrameTypeT::LIGHT;
}

void IndiCameraT::setFrameType(FrameTypeT::TypeE) {
	// TODO...
}

BinningT IndiCameraT::getSupportedMaxBinningMode() const {
	return BinningT(5, 5);
}

BinningT IndiCameraT::getBinning() const {
	return BinningT(1, 1);
}

void IndiCameraT::setBinning(const BinningT &) {
	// TODO...
}

bool IndiCameraT::isRoiSupported() const {
	return true;
}

RectT<unsigned int> IndiCameraT::getRoi() const {
	return mSelectedRoi;
}

void IndiCameraT::setRoi(const RectT<unsigned int> & selectedSubFrame) {
	LOG(debug) << "IndiCameraT::setSubFrame... subFrame: "
			<< selectedSubFrame.x() << ", " << selectedSubFrame.y() << ", w: "
			<< selectedSubFrame.width() << ", h: " << selectedSubFrame.height()
			<< std::endl;

	mSelectedRoi = selectedSubFrame;
}

void IndiCameraT::clearRoi() {
	mSelectedRoi.clear();
}

bool IndiCameraT::delayedExposueTimer() {

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

void IndiCameraT::setRoiIndi(const RectT<unsigned int> & roi) {

	INumberVectorProperty * frameVecProp = IndiHelperT::getNumberVec(
			mIndiBaseDevice, "CCD_FRAME");

	IndiHelperT::requireWritable(frameVecProp);

	INumber * pNumX = IndiHelperT::getNumber(frameVecProp, "X");
	INumber * pNumY = IndiHelperT::getNumber(frameVecProp, "Y");
	INumber * pNumWidth = IndiHelperT::getNumber(frameVecProp, "WIDTH");
	INumber * pNumHeight = IndiHelperT::getNumber(frameVecProp, "HEIGHT");

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

	mIndiClient->sendNewNumber(frameVecProp);
}

bool IndiCameraT::indiExposure() {
	LOG(debug) << "IndiCameraT - startExposure..." << std::endl;

	// TODO: Also set binning, frame type, etc...??

	// Set ROI
	try {

		setRoiIndi(mSelectedRoi);

	} catch (IndiExceptionT & exc) {
		ReportingT::reportMsg(ReportingDatasetT("IndiCamera", "Aborting exposure failed.", exc.what()));

		return false;
	}

	// Set exposure time (-> starts exposure)
	try {

		INumberVectorProperty * exposureTimeVecProp = IndiHelperT::getNumberVec(
				mIndiBaseDevice, "CCD_EXPOSURE");

		IndiHelperT::requireWritable(exposureTimeVecProp);

		INumber * exposureTimeNum = IndiHelperT::getNumber(exposureTimeVecProp,
				"CCD_EXPOSURE_VALUE");

		auto seconds = std::chrono::duration_cast < std::chrono::duration<float>
				> (mExposureTime);

		exposureTimeNum->value = seconds.count(); // In seconds

		mIndiClient->sendNewNumber(exposureTimeVecProp);

	} catch (IndiExceptionT & exc) {
		ReportingT::reportMsg(ReportingDatasetT("IndiCamera", "Unable to set exposure time.", exc.what()));

		return false;
	}

	// Wait until exposure finished or cancelled
	LOG(debug) << "Waiting for exposure to finish or cancel..."
			<< std::endl;

	std::unique_lock < std::mutex > lk(cvMutex);

	cv.wait(lk);

	return (!cancelExposureFlag.load());
}

void IndiCameraT::expose() {
	bool cancelled;
	bool lastExposure = (getLoopMode() != LoopModeT::LOOP);

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
