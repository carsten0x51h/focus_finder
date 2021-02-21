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

  // Idea to simplify the unregistering of each single listener to all the events in case the device is switched.
  void clearListeners() {
    mExposureDelayTimerUpdListeners.disconnect_all_slots();
    mExposureTimerUpdListeners.disconnect_all_slots();
    mFrameTransferUpdListeners.disconnect_all_slots();
    mExposureCancelledListeners.disconnect_all_slots();
    mExposureCycleFinishedListeners.disconnect_all_slots();
  }

  
protected:
	// To be used by startExposure, cancelExposure and actual device implementation.
	void notifyExposureCycleFinished(RectT<unsigned int> roiRect, std::shared_ptr<const ImageT> resultImage, bool lastExposure) const { mExposureCycleFinishedListeners(roiRect, resultImage, lastExposure); }
	void notifyExposureCancelled() const {
	  // HACK: Does not compile...
	  //  mExposureCancelledListeners();
	}
	void notifyExposureTimerUpd(double t) const { mExposureTimerUpdListeners(t); }
	void notifyExposureDelayTimerUpd(double t) const { mExposureDelayTimerUpdListeners(t); }
	void notifyFrameTransferUpd(double percentage) const { mFrameTransferUpdListeners(percentage); }
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CAMERA_H_ */
