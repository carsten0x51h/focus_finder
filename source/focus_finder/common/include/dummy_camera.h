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

#ifndef SOURCE_FOCUS_FINDER_COMMON_DUMMY_CAMERA_H_
#define SOURCE_FOCUS_FINDER_COMMON_DUMMY_CAMERA_H_

#include <thread>
#include <chrono>
#include <atomic>
#include <memory>

#include <functional>
#include <utility>
#include <list>

#include "camera.h"
#include "logging.h"
#include "rect.h"
#include "device_connector.h"
#include "dummy_device_connector.h"

using namespace boost; // TODO: REMOVE!
using namespace std::chrono_literals;


enum GenImageTypeE { NOISE, CCD_SIM };

// https://stackoverflow.com/questions/9404884/implementing-interfaces-in-c-with-inherited-concrete-classes
class DummyCameraT : virtual public CameraT {

public:
	DummyCameraT();
	virtual ~DummyCameraT();

	std::string getName() const;
	std::shared_ptr<DeviceConnectorT> getConnector() const;


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

	GenImageTypeE getGenImageType() const;
	void setGenImageType(GenImageTypeE genImageType);

private:
	std::shared_ptr<DummyDeviceConnectorT> mDummyConnector;

	bool delayedExposueTimer();
	bool exposureTimer();
	bool transferTimer(std::shared_ptr<ImageT> resultImage);
	void expose();

	void genNoiseImage(std::shared_ptr<ImageT> resultImage) const;
	bool loadCcdImage(std::shared_ptr<ImageT> resultImage) const;


	std::thread exposureThread;
	std::atomic<bool> cancelExposureFlag;

	std::atomic<LoopModeT::TypeE> mLoopMode;
	std::chrono::milliseconds mExposureDelay; // TODO: atomic?
	std::chrono::milliseconds mExposureTime; // TODO: atomic?

	RectT<unsigned int> mSelectedRoi;

	GenImageTypeE mGenImageType;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_DUMMY_CAMERA_H_ */
