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

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "include/indi_device_manager.h"

#include "include/logging.h"
#include "include/reporting.h"
#include "include/reporting_dataset.h"

#include "include/camera_interface.h"
#include "include/focus_interface.h"
#include "include/filter_interface.h"
#include "include/cooler.h"

#include "include/indi_camera_interface.h"
#include "include/indi_focus_interface.h"
//#include "include/indi_interface_filter.h"

#include "basedevice.h"

const std::string IndiDeviceManagerT::sDefaultIndiHostname = "localhost";
const int IndiDeviceManagerT::sDefaultIndiPort = 7624;

IndiDeviceManagerT::IndiDeviceManagerT() {
    LOG(debug) << "IndiDeviceManagerT::IndiDeviceManagerT()..."
               << std::endl;

    // TODO: Use passed "Properties" key value object.... --> setup()...
    // indiDeviceMgr->setHostname("localhost");
    // indiDeviceMgr->setPort(7624);

    // // Connect - should this be here??
    // indiDeviceMgr->connectServer();


    mIndiClient.setServer(sDefaultIndiHostname.c_str(), sDefaultIndiPort);

    // Register to listners of IndiClient
    mNewDeviceConnection = mIndiClient.registerNewDeviceListener(
            boost::bind(&IndiDeviceManagerT::newDevice, this, boost::placeholders::_1));
    mRemoveDeviceConnection = mIndiClient.registerRemoveDeviceListener(
            boost::bind(&IndiDeviceManagerT::removeDevice, this, boost::placeholders::_1));
    mNewMessageConnection = mIndiClient.registerNewMessageListener(
            boost::bind(&IndiDeviceManagerT::newMessage, this, boost::placeholders::_1, boost::placeholders::_2));
    mNewPropertyConnection = mIndiClient.registerNewPropertyListener(
            boost::bind(&IndiDeviceManagerT::newProperty, this, boost::placeholders::_1));

    //boost::signals2::connection registerNewDeviceListener(const NewDeviceListenersT::slot_type & inCallBack) {

    // Connect to server here??!?!?!

    //    void watchDevice(const char * deviceName);
//    bool connectServer();
//    bool disconnectServer();
}

IndiDeviceManagerT::~IndiDeviceManagerT() {
    // Unregister listeners
    mIndiClient.unregisterNewMessageListener(mNewMessageConnection);
    mIndiClient.unregisterNewDeviceListener(mNewDeviceConnection);
    mIndiClient.unregisterRemoveDeviceListener(mRemoveDeviceConnection);
    mIndiClient.unregisterNewPropertyListener(mNewPropertyConnection);

    // Disconnect the INDI client since this INDI device manager owns the INDI client.
    mIndiClient.disconnect();
}

void IndiDeviceManagerT::addNewDevice(INDI::BaseDevice *dp) {
    std::string indiDeviceName = dp->getDeviceName();

    LOG(debug) << "IndiDeviceManagerT::addNewDevice... device: '"
               << indiDeviceName << "', interfaces mask: " << dp->getDriverInterface() << std::endl;

    // Protect with mutex...
    std::lock_guard<std::mutex> lock(mDeviceMapMutex);

    auto deviceMapEntry = mDeviceMap.find(indiDeviceName); // std::map<...>::iterator
    bool deviceAlreadyExists = (deviceMapEntry != mDeviceMap.end());

    // Add/update device object in map that device was (re)added on the server side... -> set device "active"....
    if (!deviceAlreadyExists) {
        // INDI device is new, just create a device wrapper and add it...
        auto newDeviceWrapper = std::make_shared<IndiDeviceT>(dp, &mIndiClient);
        mDeviceMap.insert(std::make_pair(indiDeviceName, newDeviceWrapper));
        notifyDeviceAdded(newDeviceWrapper);
    } else {
        // Device already exists, update base device handle and set it active again ...
        auto existingDevice = deviceMapEntry->second;

        existingDevice->setIndiBaseDevice(dp);
        existingDevice->setAvailable(true);
        notifyDeviceAdded(existingDevice);
    }

    //TODO: Call event listener to notify about new device! Question is of generic DeviceManagerT should have a callback for newDevice()
    // and/or removeDevice()...

}

void IndiDeviceManagerT::newProperty(INDI::Property *property) {

    if (strcmp(property->getName(), "DRIVER_INFO") == 0) {
        INDI::BaseDevice *baseDevice = property->getBaseDevice();
        uint16_t driverInterface = baseDevice->getDriverInterface();

        LOG(info) << "Received DRIVER_INFO property... driver '" << baseDevice->getDeviceName()
                  << "' interface mask=" << driverInterface << std::endl;

        addNewDevice(baseDevice);
    }
}


void IndiDeviceManagerT::newMessage(INDI::BaseDevice *dp, int messageID) {
    std::string msgStr = dp->messageQueue(messageID);
    ReportingT::reportMsg(
            ReportingDatasetT("IndiServer",
                              "Message from device: '" + std::string(dp->getDeviceName()) + "'",
                              msgStr));
}

void IndiDeviceManagerT::newDevice(INDI::BaseDevice *dp) {
    // The driver interface is not known when this callback is called...
    // This info is required. Therefore, instead of listening to newDevice(),
    // newProperty() is used.
}

void IndiDeviceManagerT::removeDevice(INDI::BaseDevice *dp) {

    std::string indiDeviceName = dp->getDeviceName();

    LOG(debug) << "IndiDeviceManagerT::removeDevice... device: '"
               << indiDeviceName << "'" << std::endl;

    std::lock_guard<std::mutex> lock(mDeviceMapMutex);

    auto deviceEntry = mDeviceMap.find(indiDeviceName); // std::map<...>::iterator
    bool deviceExists = (deviceEntry != mDeviceMap.end());

    if (deviceExists) {
        LOG(debug) << "IndiDeviceManagerT::removeDevice... Setting device '" << indiDeviceName << "' to unavailable."
                   << std::endl;

        auto existingDevice = deviceEntry->second;
        existingDevice->setAvailable(false);

        notifyDeviceRemoved(existingDevice);
    } else {
        // Someting went wrong... actually we should know this device...
        LOG(warning) << "IndiDeviceManagerT::removeDevice... Device '" << indiDeviceName
                     << "' not found in local device map (but should be there). Ignoring..." << std::endl;
    }
}

DeviceManagerTypeT::TypeE IndiDeviceManagerT::getDeviceManagerType() const {
    return DeviceManagerTypeT::INDI;
}

IndiClientT &IndiDeviceManagerT::getIndiClient() {
    return mIndiClient;
}

void IndiDeviceManagerT::setHostname(const std::string &hostname) {
    // TODO: Is it a good idea to change the connection settings while connected???
    mIndiClient.setServer(hostname.c_str(), mIndiClient.getPort());
}

std::string IndiDeviceManagerT::getHostname() const {
    return const_cast<IndiClientT *>(&mIndiClient)->getHost();
}

void IndiDeviceManagerT::setPort(unsigned int port) {
    // TODO: Is it a good idea to change the connection settings while connected???
    mIndiClient.setServer(mIndiClient.getHost(), port);
}

unsigned int IndiDeviceManagerT::getPort() const {
    return const_cast<IndiClientT *>(&mIndiClient)->getPort();
}

bool IndiDeviceManagerT::isReady() const {
    // TODO: connect automatically? Actually a "isReady()" function should not do any "action"...
    //return mIndiClient.connectServer();
    return mIndiClient.isConnected();
}





//    INDI::BaseDevice * indiBaseDevice = mIndiClient.getDevice(deviceName.c_str());
//
//    return ((indiBaseDevice != nullptr && indiBaseDevice->getDriverInterface() & deviceInterfaceType) ?
//            std::make_shared<IndiCameraInterfaceT>(indiBaseDevice, const_cast<IndiClientT *>(& mIndiClient)) :
//            nullptr);
//TODO

std::shared_ptr<DeviceT> IndiDeviceManagerT::getDevice(const std::string &deviceName) {

    // Protect device map against async changes by INDI while reading...
    std::lock_guard<std::mutex> lock(mDeviceMapMutex);

    auto deviceMapEntry = mDeviceMap.find(deviceName);
    bool deviceMapEntryFound = (deviceMapEntry != mDeviceMap.end());

    return (deviceMapEntryFound ? deviceMapEntry->second : nullptr);
}


std::vector<std::shared_ptr<DeviceT> > IndiDeviceManagerT::getDevices(DeviceInterfaceTypeT::TypeE interfaceType) {

    // Protect device map against async changes by INDI while reading...
    std::lock_guard<std::mutex> lock(mDeviceMapMutex);

    std::vector<std::shared_ptr<DeviceT> > devices;

    boost::copy(
            mDeviceMap
            | boost::adaptors::map_values // -> DeviceT
            | boost::adaptors::filtered(
                    [interfaceType](std::shared_ptr<IndiDeviceT> indiDevice) {
                        return indiDevice->isInterfaceSupported(interfaceType);
                    }
            ),
            std::back_inserter(devices)
    );

    return devices;
}


std::vector<std::string> IndiDeviceManagerT::getDeviceNames(DeviceInterfaceTypeT::TypeE interfaceType) {

    std::vector<std::shared_ptr<DeviceT> > devices = getDevices(interfaceType);
    std::vector<std::string> deviceNames;

    boost::copy(
            devices
            | boost::adaptors::transformed(
                    [](auto device) { return device->getName(); }
            ),
            std::back_inserter(deviceNames)
    );

    return deviceNames;
}
