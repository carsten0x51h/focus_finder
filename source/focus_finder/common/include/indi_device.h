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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INDI_USB_DEVICE_CONNECTOR_H_
#define SOURCE_FOCUS_FINDER_COMMON_INDI_USB_DEVICE_CONNECTOR_H_ SOURCE_FOCUS_FINDER_COMMON_INDI_USB_DEVICE_CONNECTOR_H_

#include <string>
#include <atomic>
#include <thread>
#include <set>
#include <map>

#include "logging.h"
#include "device.h"
#include "indi_client.h"
#include "exception.h"

#include "basedevice.h"

#include "device_interface.h"

DEF_Exception(IndiDevice);

class IndiDeviceT : public DeviceT {

public:
    IndiDeviceT(INDI::BaseDevice *dp, IndiClientT *indiClient);

    ~IndiDeviceT() override;

    std::string getName() const override;

    void connect() override;

    void disconnect() override;

    bool isConnected() const override;

    bool isConnecting() const override;

    bool isDisconnected() const override;

    bool isDisconnecting() const override;

    DeviceConnectionStateT::TypeE getConnectionState() const override;

    std::set<DeviceInterfaceTypeT::TypeE> getSupportedInferfaces() const override;

    std::shared_ptr<DeviceInterfaceT> getInterface(DeviceInterfaceTypeT::TypeE interfaceType) override;


    // USB
    // TODO: Instead of the specific methods below, set a generic key-value config object like the Java "Properties" or similar...
    // This can be implemented in the DeviceT base class...
    //std::string getUsbDevicePort() const; // e.g. /dev/ttyUSB0 - TODO: How is this handled in Windows?
    //void setUsbDevicePort(const std::string & usbDevicePort);

    void setIndiBaseDevice(INDI::BaseDevice *indiBaseDevice);

    INDI::BaseDevice *getIndiBaseDevice();

    IndiClientT *getIndiClient();

private:
    std::shared_ptr<DeviceInterfaceT> createDeviceInterface(DeviceInterfaceTypeT::TypeE interfaceType);

    void initInterfaceMap();

    static uint16_t getIndiDeviceInterfaceMaskByDeviceType(DeviceInterfaceTypeT::TypeE deviceType);


    [[nodiscard]] DeviceConnectionStateT::TypeE getConnectionStateInternal() const;

    void connectInternal();

    void disconnectInternal();


    INDI::BaseDevice *mIndiBaseDevice;
    IndiClientT *mIndiClient;

    std::atomic<bool> mCancelConnectFlag;
    std::thread mConnectThread;

    std::map<DeviceInterfaceTypeT::TypeE, std::shared_ptr<DeviceInterfaceT> > mInterfaceMap;

};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INDI_DEVICE_CONNECTOR_H_ */
