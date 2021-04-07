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

#include "include/indi_device.h"
#include "include/indi_helper.h"
#include "include/wait_for.h"
#include "include/throw_if.h"

#include "include/indi_camera_interface.h"
#include "include/indi_focus_interface.h"
// TODO: Add further device interface includes...
//#include "include/indi_filter_interface.h"

#include <thread>
#include <chrono>
#include <atomic>

IndiDeviceT::IndiDeviceT(INDI::BaseDevice *dp,
                         IndiClientT *indiClient) :
        mIndiBaseDevice(dp), mIndiClient(indiClient), mCancelConnectFlag(false) {

    // TODO: Read initial connection status etc. from driver


    // Initialize interface wrappers
    initInterfaceMap();
}

IndiDeviceT::~IndiDeviceT() = default;

// TODO: May be moved to a sep. factory... IndiDeviceInterfaceFactoryT...
std::shared_ptr<DeviceInterfaceT> IndiDeviceT::createDeviceInterface(DeviceInterfaceTypeT::TypeE interfaceType) {
    switch (interfaceType) {
        case DeviceInterfaceTypeT::CCD: {
            auto cameraInterface = std::make_shared<IndiCameraInterfaceT>(this);
            return std::static_pointer_cast<DeviceInterfaceT>(cameraInterface);
        }
        case DeviceInterfaceTypeT::GUIDER: {
            // TODO...
            return nullptr;
        }
        case DeviceInterfaceTypeT::FOCUS: {
            auto focusInterface = std::make_shared<IndiFocusInterfaceT>(this);
            return std::static_pointer_cast<DeviceInterfaceT>(focusInterface);
        }
        case DeviceInterfaceTypeT::FILTER: {
            // TODO...
            //auto filterInterface = std::make_shared<IndiFilterInterfaceT>(this);
            //return std::static_pointer_cast<DeviceInterfaceT>(filterInterface);
            return nullptr;
        }
        case DeviceInterfaceTypeT::TELESCOPE: {
            // TODO...
            return nullptr;
        }
            // TODO / FIXME / HACK !!!!!! Add other devices!

        default: {
            std::stringstream ss;
            ss << "Cannot create device interface of type '" << DeviceInterfaceTypeT::asStr(interfaceType)
               << "'. Unknown!" << std::endl;
            throw IndiDeviceExceptionT(ss.str());
        }
    }
}

void IndiDeviceT::initInterfaceMap() {

    uint16_t indiInterfaceSupportMask = mIndiBaseDevice->getDriverInterface();

    for (size_t i = 0; i < DeviceInterfaceTypeT::_Count; ++i) {
        auto currentInterfaceType = static_cast<DeviceInterfaceTypeT::TypeE>(i);
        uint16_t currentInterfaceMask = getIndiDeviceInterfaceMaskByDeviceType(currentInterfaceType);

        if ((indiInterfaceSupportMask & currentInterfaceMask) != 0) {
            mInterfaceMap.insert(std::make_pair(currentInterfaceType, createDeviceInterface(currentInterfaceType)));
        }
    }

}

std::string IndiDeviceT::getName() const {
    return mIndiBaseDevice->getDeviceName();
}


/**
 * Defined in basedevice.h:
 * GENERAL_INTERFACE       = 0,         /// < Default interface for all INDI devices
 * TELESCOPE_INTERFACE     = (1 << 0),  // < Telescope interface, must subclass INDI::Telescope
 * CCD_INTERFACE           = (1 << 1),  // < CCD interface, must subclass INDI::CCD
 * GUIDER_INTERFACE        = (1 << 2),  // < Guider interface, must subclass INDI::GuiderInterface
 * FOCUSER_INTERFACE       = (1 << 3),  // < Focuser interface, must subclass INDI::FocuserInterface
 * FILTER_INTERFACE        = (1 << 4),  // < Filter interface, must subclass INDI::FilterInterface
 * DOME_INTERFACE          = (1 << 5),  // < Dome interface, must subclass INDI::Dome
 * GPS_INTERFACE           = (1 << 6),  // < GPS interface, must subclass INDI::GPS
 * WEATHER_INTERFACE       = (1 << 7),  // < Weather interface, must subclass INDI::Weather
 * AO_INTERFACE            = (1 << 8),  // < Adaptive Optics Interface
 * DUSTCAP_INTERFACE       = (1 << 9),  // < Dust Cap Interface
 * LIGHTBOX_INTERFACE      = (1 << 10), // < Light Box Interface
 * DETECTOR_INTERFACE      = (1 << 11), // < Detector interface, must subclass INDI::Detector
 * ROTATOR_INTERFACE       = (1 << 12), // < Rotator interface, must subclass INDI::RotatorInterface
 * SPECTROGRAPH_INTERFACE  = (1 << 13), // < Spectrograph interface
 * CORRELATOR_INTERFACE    = (1 << 14), // < Correlators (interferometers) interface
 * AUX_INTERFACE           = (1 << 15), //< Auxiliary interface
 * SENSOR_INTERFACE        = SPECTROGRAPH_INTERFACE | DETECTOR_INTERFACE | CORRELATOR_INTERFACE
 *
 * @return
 */
uint16_t
IndiDeviceT::getIndiDeviceInterfaceMaskByDeviceType(DeviceInterfaceTypeT::TypeE deviceType) {
    switch (deviceType) {
        case DeviceInterfaceTypeT::CCD:
            return INDI::BaseDevice::CCD_INTERFACE;
        case DeviceInterfaceTypeT::FOCUS:
            return INDI::BaseDevice::FOCUSER_INTERFACE;
        case DeviceInterfaceTypeT::FILTER:
            return INDI::BaseDevice::FILTER_INTERFACE;
        case DeviceInterfaceTypeT::TELESCOPE:
            return INDI::BaseDevice::TELESCOPE_INTERFACE;
        case DeviceInterfaceTypeT::AO:
            return INDI::BaseDevice::AO_INTERFACE;
        case DeviceInterfaceTypeT::AUXILIARY:
            return INDI::BaseDevice::AUX_INTERFACE;
        case DeviceInterfaceTypeT::CORRELATOR:
            return INDI::BaseDevice::CORRELATOR_INTERFACE;
        case DeviceInterfaceTypeT::DETECTOR:
            return INDI::BaseDevice::DETECTOR_INTERFACE;
        case DeviceInterfaceTypeT::DOME:
            return INDI::BaseDevice::DOME_INTERFACE;
        case DeviceInterfaceTypeT::DUSTCAP:
            return INDI::BaseDevice::DUSTCAP_INTERFACE;
        case DeviceInterfaceTypeT::GPS:
            return INDI::BaseDevice::GPS_INTERFACE;
        case DeviceInterfaceTypeT::GUIDER:
            return INDI::BaseDevice::GUIDER_INTERFACE;
        case DeviceInterfaceTypeT::LIGHTBOX:
            return INDI::BaseDevice::LIGHTBOX_INTERFACE;
        case DeviceInterfaceTypeT::ROTATOR:
            return INDI::BaseDevice::ROTATOR_INTERFACE;
        case DeviceInterfaceTypeT::SPECTROGRAPH:
            return INDI::BaseDevice::SPECTROGRAPH_INTERFACE;
        case DeviceInterfaceTypeT::WEATHER:
            return INDI::BaseDevice::WEATHER_INTERFACE;
        default:
            std::stringstream ss;
            ss << "Unsupported device interface type '" << DeviceInterfaceTypeT::asStr(deviceType) << "'.";
            throw IndiDeviceExceptionT(ss.str());
    }
}


// NOTE: This call is blocking - until success or timeout
void IndiDeviceT::connectInternal() {
    using namespace std::chrono_literals;

    std::string deviceName = mIndiBaseDevice->getDeviceName();

    LOG(debug) << "IndiDeviceT::connectInternal... deviceName='"
               << deviceName << "'" << std::endl;

    notifyDeviceConnecting(); // Notify that device starts to connect...

    mIndiClient->connectDevice(deviceName.c_str());

    auto isConnOrCancel =
            [=, this]() -> bool {
                try {
                    LOG(debug) << "mCancelConnectFlag:" << mCancelConnectFlag.load()
                               << ", getConnectionStateInternal() == DeviceConnectionStateT::CONNECTED: "
                               << (getConnectionStateInternal() == DeviceConnectionStateT::CONNECTED)
                               << std::endl;

                    return (mCancelConnectFlag || getConnectionStateInternal() == DeviceConnectionStateT::CONNECTED);
                } catch (IndiExceptionT &exc) {
                    return false;
                }
            };

// "wait for" the successful connect with a timeout.
    bool isConnected;

    try {
        wait_for(isConnOrCancel, 10000ms);
        isConnected = true;
    } catch (TimeoutExceptionT &exc) {
        notifyDeviceConnectionTimeout();
        isConnected = false;
    }

    if (isConnected) {
        notifyDeviceConnected();
    } else {
        throw DeviceExceptionT(
                "Connecting device '" + deviceName + "' failed.");
    }
}

void IndiDeviceT::connect() {

    std::string deviceName = mIndiBaseDevice->getDeviceName();

    if (isConnected() || isConnecting()) {
        LOG(debug) << "IndiDeviceT - Device '" << deviceName
                   << "' already connected or currently connecting." << std::endl;

        return;
    }

    LOG(debug) << "IndiDeviceT - Trying to connect to device '"
               << deviceName << "'..." << std::endl;

// TODO: Check somehow that thread already runs?! -> at least make sure that thread is no started multiple times!
    mCancelConnectFlag = false;

    mConnectThread = std::thread(&IndiDeviceT::connectInternal,
                                 this);
    mConnectThread.detach();
}


// NOTE: This call is blocking - until success or timeout
void IndiDeviceT::disconnectInternal() {
    using namespace std::chrono_literals;

    std::string deviceName = mIndiBaseDevice->getDeviceName();

    LOG(debug) << "IndiDeviceT::disconnectInternal... deviceName='"
               << deviceName << "'" << std::endl;

    notifyDeviceDisconnecting(); // Notify that device starts to disconnect...

    mIndiClient->disconnectDevice(deviceName.c_str());

    auto isDisconnectedLambda =
            [this]() -> bool {
                try {
                    return (getConnectionStateInternal() == DeviceConnectionStateT::DISCONNECTED);
                } catch (IndiExceptionT &exc) {
                    return false;
                }
            };

    // "wait for" the successful disconnect with a timeout.
    bool isDisconnected;

    try {
        wait_for(isDisconnectedLambda, 5000ms);
        isDisconnected = true;
    } catch (TimeoutExceptionT &exc) {
        notifyDeviceConnectionTimeout();
        isDisconnected = false;
    }

    if (isDisconnected) {
        notifyDeviceDisconnected();
    } else {
        throw IndiDeviceExceptionT(
                "Disconnecting device '" + deviceName + "' failed.");
    }
}

void IndiDeviceT::disconnect() {

    std::string deviceName = mIndiBaseDevice->getDeviceName();

    if (isDisconnected() || isDisconnecting()) {
        LOG(debug) << "IndiDeviceT - Device '" << deviceName
                   << "' already disconnected or currently disconnecting." << std::endl;

        return;
    }

    LOG(debug) << "IndiDeviceT - Trying to disconnect from device '"
               << deviceName << "'..." << std::endl;

    // TODO: Should there be another thread mDisconnectThread?
    mConnectThread = std::thread(&IndiDeviceT::disconnectInternal,
                                 this);
    mConnectThread.detach();
}

// TODO: Device connection state listener....?! -> set mConnectionState!

bool IndiDeviceT::isConnected() const {
    return (getConnectionState() == DeviceConnectionStateT::CONNECTED);
}

bool IndiDeviceT::isConnecting() const {
    return (getConnectionState() == DeviceConnectionStateT::CONNECTING);
}

bool IndiDeviceT::isDisconnected() const {
    return (getConnectionState() == DeviceConnectionStateT::DISCONNECTED);
}

bool IndiDeviceT::isDisconnecting() const {
    return (getConnectionState() == DeviceConnectionStateT::DISCONNECTING);
}

// NOTE: Throws an IndiExceptionT in case prop not found.
DeviceConnectionStateT::TypeE IndiDeviceT::getConnectionStateInternal() const {

    DeviceConnectionStateT::TypeE connState;

// Check switch property "CONNECTION" for connection state.
    ISwitchVectorProperty *deviceConnVecProp = IndiHelperT::getSwitchVec(
            mIndiBaseDevice, "CONNECTION");

    IndiHelperT::requireReadable(deviceConnVecProp);

    ISwitch *connProp = IndiHelperT::getSwitch(deviceConnVecProp, "CONNECT");

    bool connBusy = (deviceConnVecProp->s == IPS_BUSY);
    bool okOrIdle = (deviceConnVecProp->s == IPS_IDLE
                     || deviceConnVecProp->s == IPS_OK);
    bool connected = (connProp->s == ISS_ON);

    if (connected && okOrIdle) {
        connState = DeviceConnectionStateT::CONNECTED;
    } else if (!connected && okOrIdle) {
        connState = DeviceConnectionStateT::DISCONNECTED;
    } else if (connected && connBusy) {
        // Disconnecting - TODO: Is this ok?
        connState = DeviceConnectionStateT::DISCONNECTING;
    } else if (!connected && connBusy) {
        // Connecting - TODO: Is this ok?
        connState = DeviceConnectionStateT::CONNECTING;
    } else {
        LOG(warning) << "Unable to determine device connection state."
                     << std::endl;
        connState = DeviceConnectionStateT::_Count;
    }

    return connState;
}

DeviceConnectionStateT::TypeE IndiDeviceT::getConnectionState() const {

    try {
        return getConnectionStateInternal();
    } catch (IndiExceptionT &exc) {
        LOG (error) << exc.what() << std::endl;
        throw DeviceExceptionT(exc.what());
    }
}

std::set<DeviceInterfaceTypeT::TypeE> IndiDeviceT::getSupportedInferfaces() const {

    THROW_IF(IndiDevice, !mIndiBaseDevice, "mIndiBaseDevice not set!")

    // TODO: Unse mInterfaceMap instead...

    uint16_t indiInterfaceSupportMask = mIndiBaseDevice->getDriverInterface();

    std::set<DeviceInterfaceTypeT::TypeE> resultSet;

    for (size_t i = 0; i < DeviceInterfaceTypeT::_Count; ++i) {
        auto currentInterfaceType = static_cast<DeviceInterfaceTypeT::TypeE>(i);
        uint16_t currentInterfaceMask = getIndiDeviceInterfaceMaskByDeviceType(currentInterfaceType);

        if ((indiInterfaceSupportMask & currentInterfaceMask) != 0) {
            resultSet.insert(currentInterfaceType);
        }
    }

    return resultSet;
}

std::shared_ptr<DeviceInterfaceT> IndiDeviceT::getInterface(DeviceInterfaceTypeT::TypeE interfaceType) {
    if (!isInterfaceSupported(interfaceType)) {
        std::stringstream ss;
        ss << "Interface '" << DeviceInterfaceTypeT::asStr(interfaceType) << "' not supported by '" << this->getName()
           << "'." << std::endl;
        throw IndiDeviceExceptionT(ss.str());
    }

    auto deviceInterface = mInterfaceMap.find(interfaceType);

    return (deviceInterface != mInterfaceMap.end() ? deviceInterface->second : nullptr);
}


void IndiDeviceT::setIndiBaseDevice(INDI::BaseDevice *indiBaseDevice) {
    LOG(info) << "Updating INDI base device handle..." << std::endl;
    mIndiBaseDevice = indiBaseDevice;
}

INDI::BaseDevice *IndiDeviceT::getIndiBaseDevice() {
    return mIndiBaseDevice;
}

IndiClientT *IndiDeviceT::getIndiClient() {
    return mIndiClient;
}



//std::string IndiDeviceT::getUsbDevicePort() const {
//	try {
//		ITextVectorProperty * devicePortVecProp = IndiHelperT::getTextVec(
//				mIndiBaseDevice, "DEVICE_PORT");
//
//		IndiHelperT::requireReadable(devicePortVecProp);
//
//		IText * portProp = IndiHelperT::getText(devicePortVecProp, "PORT");
//
//		return portProp->text;
//
//	} catch (IndiExceptionT & exc) {
//		LOG (error) << exc.what() << std::endl;
//		throw DeviceExceptionT(exc.what());
//	}
//}
//
//void IndiDeviceT::setUsbDevicePort(
//		const std::string & inUsbDevicePort) {
//
//	try {
//		ITextVectorProperty * portDeviceVecProp = IndiHelperT::getTextVec(
//				mIndiBaseDevice, "DEVICE_PORT");
//
//		IndiHelperT::requireWritable(portDeviceVecProp);
//
//		IText * portProp = IndiHelperT::getText(portDeviceVecProp, "PORT");
//
//		std::strcpy(portProp->text /*dest*/, inUsbDevicePort.c_str() /*src*/);
//
//		mIndiClient->sendNewText(portDeviceVecProp);
//
//	} catch (IndiExceptionT & exc) {
//		throw DeviceExceptionT(
//				"Unable to set device port. Details: "
//						+ std::string(exc.what()));
//	}
//}


// FROM INDI DEVICE MANAGER
// .............................
//// TODO / FIXME: Instead of getCameraInterfaceList() > getDeviceList(Type...) ( see above!).... do not blow the IF!...+
//std::vector<std::string> IndiDeviceManagerT::getCameraInterfaceList() const {
//    return getDeviceList(DeviceInterfaceTypeT::CCD);
//}
//
//// NOTE: Returns nullptr if the device is not available
//std::shared_ptr<CameraInterfaceT> IndiDeviceManagerT::getCameraInterface(
//        const std::string & cameraName) const {
//    LOG(debug) << "IndiDeviceManagerT::getCameraInterface - cameraName: "
//               << cameraName << std::endl;
//
//    INDI::BaseDevice * indiBaseDevice = mIndiClient.getDevice(cameraName.c_str());
//
//    return ((indiBaseDevice != nullptr && indiBaseDevice->getDriverInterface() & INDI::BaseDevice::CCD_INTERFACE) ?
//            std::make_shared<IndiCameraInterfaceT>(indiBaseDevice, const_cast<IndiClientT *>(& mIndiClient)) :
//            nullptr);
//}
//
//// TODO / FIXME: Instead of getFocusInterfaceList() > getDeviceList(Type...) ( see above!).... do not blow the IF!...+
//std::vector<std::string> IndiDeviceManagerT::getFocusInterfaceList() const {
//    return getDeviceList(DeviceInterfaceTypeT::FOCUS);
//}
//
//std::shared_ptr<FocusInterfaceT> IndiDeviceManagerT::getFocusInterface(
//        const std::string & focusName) const {
//    LOG(debug) << "IndiDeviceManagerT::getFocusInterface - focusName: " << focusName
//               << std::endl;
//
//    INDI::BaseDevice * indiBaseDevice = mIndiClient.getDevice(focusName.c_str());
//
//    return ((indiBaseDevice != nullptr && indiBaseDevice->getDriverInterface() & INDI::BaseDevice::FOCUSER_INTERFACE) ?
//            std::make_shared<IndiFocusInterfaceT>(indiBaseDevice, const_cast<IndiClientT *>(& mIndiClient)) :
//            nullptr);
//}
//
//std::vector<std::string> IndiDeviceManagerT::getFilterInterfaceList() const {
//    // TODO / IDEA: We could return all devices of that type which are in the INDI drivers.xml file.
//    //              and in addition a flag if this device is currently available (loaded by the INDI server),
//    //              Then the CBX in the UI could disable the non available entries.
//    //return mIndiDeviceInterfaceFactory.getDevicesByType("Filter Wheels");
//    return getDeviceList(DeviceInterfaceTypeT::FILTER);
//}
//
//std::shared_ptr<FilterInterfaceT> IndiDeviceManagerT::getFilterInterface(
//        const std::string & filterName) const {
//    LOG(debug) << "IndiDeviceManagerT::getFilterInterface - filterName: "
//               << filterName << std::endl;
//
//    // TODO: FIXME... NEED IndiFilterInterfaceT... first
//    return nullptr;
//
////    INDI::BaseDevice * indiBaseDevice = mIndiClient.getDeviceInterface(filterName.c_str());
////
////    return (indiBaseDevice != nullptr && indiBaseDevice->getDriverInterface() & INDI::BaseDevice::FILTER_INTERFACE) ?
////           std::make_shared<IndiFilterInterfaceT>(indiBaseDevice, const_cast<IndiClientT *>(& mIndiClient)) :
////           nullptr);
//}
