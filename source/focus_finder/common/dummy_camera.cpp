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

#include <thread>
#include <chrono>
#include <atomic>
#include <memory>

#include <functional>
#include <utility>
#include <list>

#include "include/cimg_fits_io.h"

//using namespace boost; // TODO: REMOVE
using namespace std::chrono_literals;

#include "include/dummy_camera.h"

DummyCameraT::DummyCameraT() :
		cancelExposureFlag(false), mLoopMode(LoopModeT::SINGLE), mExposureDelay(
				1s), mGenImageType(CCD_SIM) {

	mDummyConnector = std::make_shared<DummyDeviceConnectorT>();
}

DummyCameraT::~DummyCameraT() {
}

/////////////////////////////////////////////////
// Device
/////////////////////////////////////////////////
std::string DummyCameraT::getName() const {
	return "MyDummyCameraName";
}

std::shared_ptr<DeviceConnectorT> DummyCameraT::getConnector() const {
	return mDummyConnector;
}

// Hardware properties
unsigned int DummyCameraT::getBitsPerPixel() const {
	return 16;
}

SizeT<float> DummyCameraT::getPixelSize() const {
	return SizeT<float>(57 /*um*/, 57 /*um*/);
}

SizeT<unsigned int> DummyCameraT::getMaxResolution() const {
	return SizeT<unsigned int>(2500 /*x pixel*/, 1500 /*y pixel*/);
}


std::chrono::milliseconds DummyCameraT::getMinExposureTime() const {
	return 100ms;
}

std::chrono::milliseconds DummyCameraT::getMaxExposureTime() const {
	return 3600s; // 1h
}

// Camera
// Exposure related properties
void DummyCameraT::startExposure() {
	if (isExposureRunning()) {
		throw CameraExceptionT("Exposure already running.");
	}

	// TODO: Check if connected...
	LOG(debug)
	<< "DummyCameraT - startExposure..." << std::endl;

	cancelExposureFlag = false;

	exposureThread = std::thread(&DummyCameraT::expose, this);
	exposureThread.detach();
}

void DummyCameraT::cancelExposure() {
	LOG(debug)
	<< "DummyCameraT - cancelExposure..." << std::endl;

	cancelExposureFlag = true;
}

bool DummyCameraT::isExposureRunning() {
	// TODO: Check if thread is still running.... future...? wait_for()...?
	return false;
}

std::chrono::milliseconds DummyCameraT::getExposureTime() const {
	return mExposureTime;
}

void DummyCameraT::setExposureTime(const std::chrono::milliseconds & exposureTime) {
	mExposureTime = exposureTime;
}


void DummyCameraT::setExposureDelay(
		const std::chrono::milliseconds & exposureDelay) {
	mExposureDelay = exposureDelay;
}

std::chrono::milliseconds DummyCameraT::getExposureDelay() const {
	return mExposureDelay;
}

LoopModeT::TypeE DummyCameraT::getLoopMode() const {
	return mLoopMode.load();
}

void DummyCameraT::setLoopMode(LoopModeT::TypeE loopMode) {
	mLoopMode = loopMode;
}

std::list<FrameTypeT::TypeE> DummyCameraT::getSupportedFrameTypes() const {
	return {FrameTypeT::LIGHT, FrameTypeT::BIAS, FrameTypeT::DARK, FrameTypeT::FLAT};
}

FrameTypeT::TypeE DummyCameraT::getFrameType() const {
	return FrameTypeT::LIGHT;
}

void DummyCameraT::setFrameType(FrameTypeT::TypeE) {
	// TODO...
}

BinningT DummyCameraT::getSupportedMaxBinningMode() const {
	return BinningT(5, 5);
}

BinningT DummyCameraT::getBinning() const {
	return BinningT(1, 1);
}

void DummyCameraT::setBinning(const BinningT &) {
	// TODO...
}

bool DummyCameraT::isRoiSupported() const {
	return true;
}

RectT<unsigned int> DummyCameraT::getRoi() const {
	return mSelectedRoi;
}

void DummyCameraT::setRoi(const RectT<unsigned int> & selectedSubFrame) {
	LOG(debug)
	<< "DummyCameraT::setSubFrame... subFrame: " << selectedSubFrame.x() << ", "
			<< selectedSubFrame.y() << ", w: " << selectedSubFrame.width()
			<< ", h: " << selectedSubFrame.height() << std::endl;

	mSelectedRoi = selectedSubFrame;
}

void DummyCameraT::clearRoi() {
	mSelectedRoi.clear();
}

bool DummyCameraT::delayedExposueTimer() {
	// Delay
	int exosureDelay = mExposureDelay.count() / 100;

	while (exosureDelay > 0) {
		LOG(debug)
		<< "Next exposure in '" << exosureDelay << "'..." << std::endl;

		if (cancelExposureFlag.load()) {
			return false;
		}

		std::this_thread::sleep_for(100ms);
		--exosureDelay;
	}
	return true;
}

bool DummyCameraT::exposureTimer() {
	// Start exposure timer...
	int exposureTime = mExposureTime.count() / 100;

	while (exposureTime > 0) {
		LOG(debug)
		<< "Exposure is running '" << exposureTime << "'..." << std::endl;

		if (cancelExposureFlag.load()) {
			return false;
		}

		notifyExposureTimerUpd(exposureTime);

		std::this_thread::sleep_for(100ms);
		--exposureTime;
	}
	return true;
}

void DummyCameraT::setGenImageType(GenImageTypeE genImageType) {
	mGenImageType = genImageType;
}

GenImageTypeE DummyCameraT::getGenImageType() const {
	return mGenImageType;
}

void DummyCameraT::genNoiseImage(std::shared_ptr<ImageT> resultImage) const {
	if (mSelectedRoi.isSet()) {
		// In case sub frame is selected...
		resultImage->assign(mSelectedRoi.width(), mSelectedRoi.height(),
				1 /*z*/, 1 /*c*/);
	} else {
		// TODO: Binning....
		// In case no sub frame is selected...
		const int w = get<0>(getMaxResolution());
		const int h = get<1>(getMaxResolution());
		resultImage->assign(w, h, 1 /*z*/, 1 /*c*/);
	}

	// Generate random image
	resultImage->rand(0.0, 255.0);
}

bool DummyCameraT::loadCcdImage(std::shared_ptr<ImageT> resultImage) const {
	// Load test image
	try {
		const std::string filename = "../test_data/ccd_simulator.fits";
		//const std::string filename = "../test_data/focus_test/focus_test_604.fit";

		long bitsPerPix = 0;

		CImgFitsIOHelperT::readFits(resultImage.get(), filename, &bitsPerPix);

		// TODO: Check if 'mSelectedRoi' is within image dimensions

		LOG(debug)
		<< "DummyCameraT::loadCcdImage...mSelectedRoi.isSet()? "
				<< mSelectedRoi.isSet() << ", x: " << mSelectedRoi.x()
				<< ", y: " << mSelectedRoi.y() << ", w: "
				<< mSelectedRoi.width() << ", h: " << mSelectedRoi.height()
				<< std::endl;

		if (mSelectedRoi.isSet()) {
			resultImage->crop(mSelectedRoi.x(), mSelectedRoi.y(),
					mSelectedRoi.x() + mSelectedRoi.width(),
					mSelectedRoi.y() + mSelectedRoi.height());
		}
	} catch (FitsIOExceptionT & exc) {
		LOG(fatal)
		<< "DummyCamera failed to read FITS file: " << exc.what() << std::endl;
		return false;
	}
	return true;
}

bool DummyCameraT::transferTimer(std::shared_ptr<ImageT> resultImage) {
// Transfer
	static const int transferDelay = 2;
	int transferDelayCounter = 0;
	bool res;

	LOG(debug)
	<< "Transfer started..." << std::endl;

	while (transferDelayCounter <= transferDelay) {
		double percentage = ((double) transferDelayCounter
				/ (double) transferDelay) * 100.0;

		notifyFrameTransferUpd(percentage); 	// Transfer in progress

		LOG(debug)
		<< "Transfer in progress..." << percentage << "%" << std::endl;

		std::this_thread::sleep_for(1s);
		++transferDelayCounter;
	}

	switch (mGenImageType) {
	case NOISE:
		genNoiseImage(resultImage);
		res = true;
		break;

	case CCD_SIM:
		res = loadCcdImage(resultImage);
		break;

	default:
		LOG(warning)
		<< "Invalid image type." << std::endl;
		return false;
	}

	LOG(debug)
	<< "DummyCameraT::transferTimer... resultImage - w: "
			<< resultImage->width() << ", h: " << resultImage->height()
			<< std::endl;

	LOG(debug)
	<< "Transfer finished..." << std::endl;

	return res;
}

void DummyCameraT::expose() {
	bool cancelled;
	bool lastExposure = (getLoopMode() != LoopModeT::LOOP);

	do {
		std::shared_ptr<ImageT> resultImage = std::make_shared<ImageT>();

		// https://stackoverflow.com/questions/7925479/if-argument-evaluation-order
		// https://stackoverflow.com/questions/18450585/what-is-the-order-of-evaluation-of-statements-in-a-if-bracket-if
		cancelled = !delayedExposueTimer() || !exposureTimer()
				|| !transferTimer(resultImage);
		lastExposure = (getLoopMode() != LoopModeT::LOOP);

		if (cancelled) {
			LOG(debug)
			<< "Exposure cancelled." << std::endl;
			notifyExposureCancelled();
		} else {
			LOG(debug)
			<< "Exposure finished." << std::endl;
			notifyExposureCycleFinished(mSelectedRoi, resultImage,
					lastExposure);
		}

	} while (!lastExposure && !cancelled);
}

