/*
 * indi_camera.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INDI_CAMERA_H_
#define SOURCE_FOCUS_FINDER_COMMON_INDI_CAMERA_H_

#include <thread>
#include <chrono>
#include <atomic>
#include <memory>

#include <functional>
#include <utility>
#include <list>

#include <condition_variable>

#include <boost/signals2.hpp>

#include "camera.h"
#include "logging.h"
#include "rect.h"
#include "image.h"
#include "device_connector.h"
#include "indi_usb_device_connector.h"
#include "indi_client.h"

// INDI
#include "basedevice.h"


using namespace boost; // TODO: REMOVE!
using namespace std::chrono_literals;

// https://stackoverflow.com/questions/9404884/implementing-interfaces-in-c-with-inherited-concrete-classes
class IndiCameraT : virtual public CameraT {

public:
	IndiCameraT(INDI::BaseDevice *dp, IndiClientT * indiClient);
	virtual ~IndiCameraT();

	std::string getName() const;
	std::shared_ptr<DeviceConnectorT> getConnector() const;
	DeviceTypeT getDeviceType() const;


	unsigned int getBitsPerPixel() const;
	SizeT<float> getPixelSize() const;
	SizeT<unsigned int> getMaxResolution() const;

	std::chrono::milliseconds getMinExposureTime() const;
	std::chrono::milliseconds getMaxExposureTime() const;


	void startExposure();
	void cancelExposure();
	bool isExposureRunning();

	std::chrono::milliseconds getExposureTime() const;
	void setExposureTime(const std::chrono::milliseconds & exposureTime);

	void setExposureDelay(const std::chrono::milliseconds & exposureDelay);
	std::chrono::milliseconds getExposureDelay() const;

	LoopModeT::TypeE getLoopMode() const;
	void setLoopMode(LoopModeT::TypeE loopMode);

	std::list<FrameTypeT::TypeE> getSupportedFrameTypes() const;
	FrameTypeT::TypeE getFrameType() const;
	void setFrameType(FrameTypeT::TypeE frameType);

	BinningT getSupportedMaxBinningMode() const;
	BinningT getBinning() const;
	void setBinning(const BinningT & binning);

	bool isRoiSupported() const;
	RectT<unsigned int> getRoi() const;
	void setRoi(const RectT<unsigned int> &);
	void clearRoi();

private:
	void setRoiIndi(const RectT<unsigned int> & roi);


	// TODO: This will probably later be IndiDeviceConnectorT - a super
	//       class of IndiUsbDeviceConnectorT and IndiEthernetDeviceConnectorT.
	std::shared_ptr<IndiUsbDeviceConnectorT> mIndiConnector;

	std::shared_ptr<ImageT> convertIndiBlobToCImg(IBLOB* iblob) const;
	void newNumber(INumberVectorProperty * nvp);
	void newBlob(IBLOB* blob);
    void newSwitch(ISwitchVectorProperty* svp);

	bool indiExposure();
	bool delayedExposueTimer();
//	bool exposureTimer();
//	bool transferTimer(std::shared_ptr<ImageT> resultImage);
	void expose();


	INDI::BaseDevice *mIndiBaseDevice;
	IndiClientT * mIndiClient;

	boost::signals2::connection mNewNumberConnection;
	boost::signals2::connection mNewBlobConnection;
    boost::signals2::connection mNewSwitchConnection;

	std::thread exposureThread;
	std::atomic<bool> cancelExposureFlag;
    std::atomic<bool> mIsExposureRunning;

	std::atomic<LoopModeT::TypeE> mLoopMode;
	std::chrono::milliseconds mExposureDelay; // TODO: atomic?
	std::chrono::milliseconds mExposureTime; // TODO: atomic?

	RectT<unsigned int> mSelectedRoi;

	std::shared_ptr<ImageT> mResultImage;
	std::condition_variable cv;
	std::mutex cvMutex;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INDI_CAMERA_H_ */
