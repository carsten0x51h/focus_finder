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

#include "include/camera.h"
#include "include/focus.h"
#include "include/filter.h"
#include "include/cooler.h"

#include "include/indi_device_factory.h"

#include "basedevice.h"

const std::string IndiDeviceManagerT::sDefaultIndiHostname = "localhost";
const int IndiDeviceManagerT::sDefaultIndiPort = 7624;

IndiDeviceManagerT::IndiDeviceManagerT() {
	LOG(debug) << "IndiDeviceManagerT::IndiDeviceManagerT()..."
			<< std::endl;

	// TODO: Read cfg?!?!?!?
	// TODO: Read config, maybe supply path? Or ask static instance? Maybe move this config part into sep. function?
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

	
	// Disconnect the INDI client since this INDI device manager owns the INDI client.
	mIndiClient.disconnect();
  }



void IndiDeviceManagerT::newMessage(INDI::BaseDevice *dp, int messageID) {
	std::string msgStr = dp->messageQueue(messageID);
	ReportingT::reportMsg(
			ReportingDatasetT("IndiServer",
					"Message from device: '" + std::string(dp->getDeviceName()) + "'",
					msgStr));
}

void IndiDeviceManagerT::newDevice(INDI::BaseDevice *dp) {
	LOG(debug) << "IndiDeviceManagerT::newDevice... device: '"
			<< dp->getDeviceName() << "'" << std::endl;

	// TODO: Based on the entries in the XML file, classify the different devices and
	// create the respecitve device objects. (lookup by name? -> Group name -> type)

// TODO / FIXME: Stop reading the drivers.xml file. Instead use the driver information (driver device type or similar - see code from PHD2...) - indi_config.cpp
	
	std::string deviceName = dp->getDeviceName();

	auto it = mDeviceMap.find(deviceName);

	if (it == mDeviceMap.end()) {
		auto device = mIndiDeviceFactory.createDevice(dp,
				const_cast<IndiClientT*>(&mIndiClient));

		LOG(debug) << "Adding new device '" << deviceName << "', device="
				<< device << std::endl;

		mDeviceMap.insert(std::make_pair(deviceName, device));
	}
}

void IndiDeviceManagerT::removeDevice(INDI::BaseDevice *dp) {
	LOG(debug) << "IndiDeviceManagerT::removeDevice... device: '"
			<< dp->getDeviceName() << "'" << std::endl;

	// TODO...
}

DeviceManagerTypeT::TypeE IndiDeviceManagerT::getDeviceManagerType() const {
  return DeviceManagerTypeT::INDI;
}

IndiClientT & IndiDeviceManagerT::getIndiClient() {
  return mIndiClient;
}

std::shared_ptr<DeviceT> IndiDeviceManagerT::getDevice(
		const std::string & deviceName) const {
	LOG(debug) << "IndiDeviceManagerT::getDevice - deviceName: "
			<< deviceName << std::endl;

	// DEBUG BEGIN
	LOG(trace) << "DeviceMap" << std::endl;
	for (const auto & e : mDeviceMap) {
		LOG(trace) << "  -> DeviceName: " << e.first << " -> device: "
				<< e.second << std::endl;
	}
	// DEBUG END

	// TODO: Lookup device in mDeviceMap
	auto it = mDeviceMap.find(deviceName);

	return (it != mDeviceMap.end() ? it->second : nullptr);
}

std::vector<std::string> IndiDeviceManagerT::getDeviceList(
		DeviceTypeT::TypeE deviceType) const {
	std::vector < std::string > res;

	boost::copy(
			mDeviceMap
					| boost::adaptors::map_values // -> DeviceT
					| boost::adaptors::filtered(
							[](const auto & device) {return device != nullptr;})
					| boost::adaptors::filtered(
							[=](const auto & device) {return device->getDeviceType() == deviceType;})
					| boost::adaptors::transformed(
							[](const auto & device) {return device->getName();}),
			std::back_inserter(res));

	return res;
}

std::vector<std::string> IndiDeviceManagerT::getCameraList() const {
	// TODO / IDEA: We could return all devices of that type which are in the INDI drivers.xml file.
	//              and in addition a flag if this device is currently available (loaded by the INDI server),
	//              Then the CBX in the UI could disable the non available entries.
	//return mIndiDeviceFactory.getDevicesByType("CCDs");
	return getDeviceList(DeviceTypeT::CAMERA);
}

// NOTE: Returns nullptr if the device is not available
std::shared_ptr<CameraT> IndiDeviceManagerT::getCamera(
		const std::string & cameraName) const {
	LOG(debug) << "IndiDeviceManagerT::getCamera - cameraName: "
			<< cameraName << std::endl;
	return std::static_pointer_cast < CameraT > (getDevice(cameraName));
}

std::vector<std::string> IndiDeviceManagerT::getFocusList() const {
	// TODO / IDEA: We could return all devices of that type which are in the INDI drivers.xml file.
	//              and in addition a flag if this device is currently available (loaded by the INDI server),
	//              Then the CBX in the UI could disable the non available entries.
	//return mIndiDeviceFactory.getDevicesByType("Focusers");
	return getDeviceList(DeviceTypeT::FOCUS);
}

std::shared_ptr<FocusT> IndiDeviceManagerT::getFocus(
		const std::string & focusName) const {
	LOG(debug) << "IndiDeviceManagerT::getFocus - focusName: " << focusName
			<< std::endl;
	return std::static_pointer_cast < FocusT > (getDevice(focusName));
}

std::vector<std::string> IndiDeviceManagerT::getFilterList() const {
	// TODO / IDEA: We could return all devices of that type which are in the INDI drivers.xml file.
	//              and in addition a flag if this device is currently available (loaded by the INDI server),
	//              Then the CBX in the UI could disable the non available entries.
	//return mIndiDeviceFactory.getDevicesByType("Filter Wheels");
	return getDeviceList(DeviceTypeT::FILTER);
}

std::shared_ptr<FilterT> IndiDeviceManagerT::getFilter(
		const std::string & filterName) const {
	LOG(debug) << "IndiDeviceManagerT::getFilter - filterName: "
			<< filterName << std::endl;
	return std::static_pointer_cast < FilterT > (getDevice(filterName));
}

void IndiDeviceManagerT::setHostname(const std::string & hostname) {
	// TODO: Is it a good idea to change the connection settings while connected???
	mIndiClient.setServer(hostname.c_str(), mIndiClient.getPort());
}

std::string IndiDeviceManagerT::getHostname() const {
	return const_cast<IndiClientT*>(&mIndiClient)->getHost();
}

void IndiDeviceManagerT::setPort(unsigned int port) {
	// TODO: Is it a good idea to change the connection settings while connected???
	mIndiClient.setServer(mIndiClient.getHost(), port);
}

unsigned int IndiDeviceManagerT::getPort() const {
	return const_cast<IndiClientT*>(&mIndiClient)->getPort();
}

bool IndiDeviceManagerT::isReady() {
  // TODO: Avoid connect if already connected...
  return mIndiClient.connectServer();
}
