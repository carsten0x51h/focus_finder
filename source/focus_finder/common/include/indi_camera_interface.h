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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INDI_CAMERA_INTERFACE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INDI_CAMERA_INTERFACE_H_

#include <thread>
#include <chrono>
#include <atomic>
#include <memory>

#include <functional>
#include <utility>
#include <list>

#include <condition_variable>

#include <boost/signals2.hpp>

#include "camera_interface.h"
#include "logging.h"
#include "rect.h"
#include "image.h"
#include "indi_device.h"
#include "indi_client.h"

// INDI
#include "basedevice.h"


using namespace boost; // TODO: REMOVE!
using namespace std::chrono_literals;

// https://stackoverflow.com/questions/9404884/implementing-interfaces-in-c-with-inherited-concrete-classes
class IndiCameraInterfaceT : virtual public CameraInterfaceT {

public:
    IndiCameraInterfaceT(IndiDeviceT *indiDevice);

    virtual ~IndiCameraInterfaceT();

    std::string getName() const;

    DeviceT *getParentDevice();

    unsigned int getBitsPerPixel() const;

    SizeT<float> getPixelSize() const;

    SizeT<unsigned int> getMaxResolution() const;

    std::chrono::milliseconds getMinExposureTime() const;

    std::chrono::milliseconds getMaxExposureTime() const;


    void startExposure();

    void cancelExposure();

    bool isExposureRunning();

    std::chrono::milliseconds getExposureTime() const;

    void setExposureTime(const std::chrono::milliseconds &exposureTime);

    void setExposureDelay(const std::chrono::milliseconds &exposureDelay);

    std::chrono::milliseconds getExposureDelay() const;

    LoopModeT::TypeE getLoopMode() const;

    void setLoopMode(LoopModeT::TypeE loopMode);

    std::list<FrameTypeT::TypeE> getSupportedFrameTypes() const;

    FrameTypeT::TypeE getFrameType() const;

    void setFrameType(FrameTypeT::TypeE frameType);

    BinningT getSupportedMaxBinningMode() const;

    BinningT getBinning() const;

    void setBinning(const BinningT &binning);

    bool isRoiSupported() const;

    RectT<unsigned int> getRoi() const;

    void setRoi(const RectT<unsigned int> &);

    void clearRoi();

private:
    void setRoiIndi(const RectT<unsigned int> &roi);


    // TODO: This will probably later be IndiDeviceConnectorT - a super
    //       class of IndiDeviceT and IndiEthernetDeviceConnectorT.
    //std::shared_ptr<IndiDeviceT> mIndiConnector;

    std::shared_ptr<ImageT> convertIndiBlobToCImg(IBLOB *iblob) const;

    void newNumber(INumberVectorProperty *nvp);

    void newBlob(IBLOB *blob);

    void newSwitch(ISwitchVectorProperty *svp);

    bool indiExposure();

    bool delayedExposueTimer();

//	bool exposureTimer();
//	bool transferTimer(std::shared_ptr<ImageT> resultImage);
    void expose();


    IndiDeviceT *mIndiDevice;

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

#endif /* SOURCE_FOCUS_FINDER_COMMON_INDI_CAMERA_INTERFACE_H_ */
