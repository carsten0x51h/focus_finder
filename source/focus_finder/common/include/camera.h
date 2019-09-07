/*
 * camera.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CAMERA_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CAMERA_H_

#include <chrono>
#include <tuple>
#include <memory>

#include <boost/signals2.hpp>


#include "device.h"
#include "exception.h"
#include "image.h"
#include "point.h"
#include "rect.h"
#include "binning.h"
#include "size.h"

DEF_Exception(Camera);

// TODO: Sep. header file for FrameType...!!!
/**
 * FrameType
 */
struct FrameTypeT {
  enum TypeE {
    LIGHT,
    BIAS,
    DARK,
    FLAT,
    _Count
  };

  static const char * asStr(const TypeE & inType) {
    switch (inType) {
    	case LIGHT: return "LIGHT";
    	case BIAS: return "BIAS";
    	case DARK: return "DARK";
    	case FLAT: return "FLAT";
    	default: return "<?>";
    }
  }
};

/**
 * LoopModeT
 */
struct LoopModeT {
  enum TypeE {
    SINGLE,
    LOOP,
    _Count
  };

  static const char * asStr(const TypeE & inType) {
    switch (inType) {
    	case SINGLE: return "SINGLE";
    	case LOOP: return "LOOP";
    	default: return "<?>";
    }
  }
};



using namespace boost;

class CameraT : public DeviceT {
private:
	typedef signals2::signal<void (double /*secs. left*/)> ExposureDelayTimerUpdListenersT;
	typedef signals2::signal<void (double /*secs. left*/)> ExposureTimerUpdListenersT;
	typedef signals2::signal<void (double /*perc.*/)> FrameTransferUpdListenersT;
	typedef signals2::signal<void (void)> ExposureCancelledListenersT;
	typedef signals2::signal<void (RectT<unsigned int>, std::shared_ptr<const ImageT>, bool)> ExposureCycleFinishedListenersT;

	ExposureDelayTimerUpdListenersT mExposureDelayTimerUpdListeners;
	ExposureTimerUpdListenersT mExposureTimerUpdListeners;
	FrameTransferUpdListenersT mFrameTransferUpdListeners;
	ExposureCancelledListenersT mExposureCancelledListeners;
	ExposureCycleFinishedListenersT mExposureCycleFinishedListeners;

	// We do not want device copies
	CameraT(const CameraT &);
	CameraT & operator=(const CameraT &);


public:
	CameraT() {}

	// Hardware properties
	virtual unsigned int getBitsPerPixel() const = 0;
	virtual SizeT<float> getPixelSize() const = 0;
	virtual SizeT<unsigned int> getMaxResolution() const = 0;

	virtual std::chrono::milliseconds getMinExposureTime() const = 0;
	virtual std::chrono::milliseconds getMaxExposureTime() const = 0;

	// Exposure related properties
	virtual void startExposure() = 0;
	virtual void cancelExposure() = 0;
	virtual bool isExposureRunning() = 0;

	virtual std::chrono::milliseconds getExposureTime() const = 0;
	virtual void setExposureTime(const std::chrono::milliseconds & exposureTime) = 0;

	virtual void setExposureDelay(const std::chrono::milliseconds & exposureDelay) = 0;
	virtual std::chrono::milliseconds getExposureDelay() const = 0;

	virtual LoopModeT::TypeE getLoopMode() const = 0;
	virtual void setLoopMode(LoopModeT::TypeE loopMode) = 0;


	virtual std::list<FrameTypeT::TypeE> getSupportedFrameTypes() const = 0;
	virtual FrameTypeT::TypeE getFrameType() const = 0;
	virtual void setFrameType(FrameTypeT::TypeE frameType) = 0;

	virtual BinningT getSupportedMaxBinningMode()const = 0;
	virtual BinningT getBinning() const = 0;
	virtual void setBinning(const BinningT & binning) = 0;

	virtual bool isRoiSupported() const = 0;
	virtual RectT<unsigned int> getRoi() const = 0;
	virtual void setRoi(const RectT<unsigned int> & rect) = 0;
	virtual void clearRoi() = 0;


	// Internal?
	//virtual bool isCompressed() = 0; - internal?
	//virtual bool getShutterState() = 0 - internal?



	// Register on events
	signals2::connection registerExposureCycleFinishedListener(const ExposureCycleFinishedListenersT::slot_type & inCallBack) {
		return mExposureCycleFinishedListeners.connect(inCallBack);
	}

	template <class T> void unregisterExposureCycleFinishedListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

	signals2::connection registerExposureCancelledListener(const ExposureCancelledListenersT::slot_type & inCallBack) {
		return mExposureCancelledListeners.connect(inCallBack);
	}
	template <class T> void unregisterExposureCancelledListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

	signals2::connection registerExposureDelayTimerUpdListener(const ExposureDelayTimerUpdListenersT::slot_type & inCallBack) {
		return mExposureDelayTimerUpdListeners.connect(inCallBack);
	}
	template <class T> void unregisterExposureDelayTimerUpdListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

	signals2::connection registerExposureTimerUpdListener(const ExposureTimerUpdListenersT::slot_type & inCallBack) {
		return mExposureTimerUpdListeners.connect(inCallBack);
	}
	template <class T> void unregisterExposureTimerUpdListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

	signals2::connection registerFrameTransferUpdListener(const FrameTransferUpdListenersT::slot_type & inCallBack) {
		return mFrameTransferUpdListeners.connect(inCallBack);
	}
	template <class T> void unregisterFrameTransferUpdListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

protected:
	// To be used by startExposure, cancelExposure and actual device implementation.
	void notifyExposureCycleFinished(RectT<unsigned int> roiRect, std::shared_ptr<const ImageT> resultImage, bool lastExposure) const { mExposureCycleFinishedListeners(roiRect, resultImage, lastExposure); }
	void notifyExposureCancelled() const { mExposureCancelledListeners(); }
	void notifyExposureTimerUpd(double t) const { mExposureTimerUpdListeners(t); }
	void notifyExposureDelayTimerUpd(double t) const { mExposureDelayTimerUpdListeners(t); }
	void notifyFrameTransferUpd(double percentage) const { mFrameTransferUpdListeners(percentage); }
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CAMERA_H_ */
