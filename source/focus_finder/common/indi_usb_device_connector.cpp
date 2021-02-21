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

#include "include/indi_usb_device_connector.h"
#include "include/indi_helper.h"
#include "include/wait_for.h"


#include <thread>
#include <chrono>
#include <atomic>

IndiUsbDeviceConnectorT::IndiUsbDeviceConnectorT(INDI::BaseDevice *dp,
		IndiClientT * indiClient) :
		mIndiBaseDevice(dp), mIndiClient(indiClient), mCancelConnectFlag(false) {

	// TODO: Read initial connection status etc. from driver
}

IndiUsbDeviceConnectorT::~IndiUsbDeviceConnectorT() {
}


// NOTE: This call is blocking - until success or timeout
void IndiUsbDeviceConnectorT::connectInternal() {
  using namespace std::chrono_literals;
  
	std::string deviceName = mIndiBaseDevice->getDeviceName();

	LOG(debug) << "IndiUsbDeviceConnectorT::connectInternal... deviceName='"
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
				} catch (IndiExceptionT & exc) {
					return false;
				}
			};

// "wait for" the successful connect with a timeout.
	bool isConnected;

	try {
		wait_for(isConnOrCancel, 10000ms);
		isConnected = true;
	} catch (TimeoutExceptionT & exc) {
		notifyDeviceConnectionTimeout();
		isConnected = false;
	}

	if (isConnected) {
		notifyDeviceConnected();
	} else {
		throw DeviceConnectorExceptionT(
				"Connecting device '" + deviceName + "' failed.");
	}
}

void IndiUsbDeviceConnectorT::connect() {

	std::string deviceName = mIndiBaseDevice->getDeviceName();

	if (isConnected() || isConnecting()) {
		LOG(debug) << "IndiUsbDeviceConnectorT - Device '" << deviceName
				<< "' already connected or currently connecting." << std::endl;

		return;
	}

	LOG(debug) << "IndiUsbDeviceConnectorT - Trying to connect to device '"
			<< deviceName << "'..." << std::endl;

// TODO: Check somehow that thread already runs?! -> at least make sure that thread is no started multiple times!
	mCancelConnectFlag = false;

	mConnectThread = std::thread(&IndiUsbDeviceConnectorT::connectInternal,
			this);
	mConnectThread.detach();
}


// NOTE: This call is blocking - until success or timeout
void IndiUsbDeviceConnectorT::disconnectInternal() {
    using namespace std::chrono_literals;

	std::string deviceName = mIndiBaseDevice->getDeviceName();

	LOG(debug) << "IndiUsbDeviceConnectorT::disconnectInternal... deviceName='"
			<< deviceName << "'" << std::endl;

	notifyDeviceDisconnecting(); // Notify that device starts to disconnect...

	mIndiClient->disconnectDevice(deviceName.c_str());

	auto isDisconnectedLambda =
			[=]() -> bool {
				try {
					return (getConnectionStateInternal() == DeviceConnectionStateT::DISCONNECTED);
				} catch (IndiExceptionT & exc) {
					return false;
				}
			};

	// "wait for" the successful disconnect with a timeout.
	bool isDisconnected;

	try {
		wait_for(isDisconnectedLambda, 5000ms);
		isDisconnected = true;
	} catch (TimeoutExceptionT & exc) {
		notifyDeviceConnectionTimeout();
		isDisconnected = false;
	}

	if (isDisconnected) {
		notifyDeviceDisconnected();
	} else {
		throw DeviceConnectorExceptionT(
				"Disconnecting device '" + deviceName + "' failed.");
	}
}

void IndiUsbDeviceConnectorT::disconnect() {

	std::string deviceName = mIndiBaseDevice->getDeviceName();

	if (isDisconnected() || isDisconnecting()) {
		LOG(debug) << "IndiUsbDeviceConnectorT - Device '" << deviceName
				<< "' already disconnected or currently disconnecting." << std::endl;

		return;
	}

	LOG(debug) << "IndiUsbDeviceConnectorT - Trying to disconnect from device '"
			<< deviceName << "'..." << std::endl;

	// TODO: Should there be another thread mDisconnectThread?
	mConnectThread = std::thread(&IndiUsbDeviceConnectorT::disconnectInternal,
			this);
	mConnectThread.detach();
}

// TODO: Device connection state listener....?! -> set mConnectionState!

bool IndiUsbDeviceConnectorT::isConnected() const {
	return (getConnectionState() == DeviceConnectionStateT::CONNECTED);
}

bool IndiUsbDeviceConnectorT::isConnecting() const {
	return (getConnectionState() == DeviceConnectionStateT::CONNECTING);
}

bool IndiUsbDeviceConnectorT::isDisconnected() const {
	return (getConnectionState() == DeviceConnectionStateT::DISCONNECTED);
}

bool IndiUsbDeviceConnectorT::isDisconnecting() const {
	return (getConnectionState() == DeviceConnectionStateT::DISCONNECTING);
}

// NOTE: Throws an IndiExceptionT in case prop not found.
DeviceConnectionStateT::TypeE IndiUsbDeviceConnectorT::getConnectionStateInternal() const {

	DeviceConnectionStateT::TypeE connState;

// Check switch property "CONNECTION" for connection state.
	ISwitchVectorProperty * deviceConnVecProp = IndiHelperT::getSwitchVec(
			mIndiBaseDevice, "CONNECTION");

	IndiHelperT::requireReadable(deviceConnVecProp);

	ISwitch * connProp = IndiHelperT::getSwitch(deviceConnVecProp, "CONNECT");

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

DeviceConnectionStateT::TypeE IndiUsbDeviceConnectorT::getConnectionState() const {

	try {
		return getConnectionStateInternal();
	} catch (IndiExceptionT & exc) {
		LOG (error) << exc.what() << std::endl;
		throw DeviceConnectorExceptionT(exc.what());
	}
}

std::string IndiUsbDeviceConnectorT::getUsbDevicePort() const {
	try {
		ITextVectorProperty * devicePortVecProp = IndiHelperT::getTextVec(
				mIndiBaseDevice, "DEVICE_PORT");

		IndiHelperT::requireReadable(devicePortVecProp);

		IText * portProp = IndiHelperT::getText(devicePortVecProp, "PORT");

		return portProp->text;

	} catch (IndiExceptionT & exc) {
		LOG (error) << exc.what() << std::endl;
		throw DeviceConnectorExceptionT(exc.what());
	}
}

void IndiUsbDeviceConnectorT::setUsbDevicePort(
		const std::string & inUsbDevicePort) {

	try {
		ITextVectorProperty * portDeviceVecProp = IndiHelperT::getTextVec(
				mIndiBaseDevice, "DEVICE_PORT");

		IndiHelperT::requireWritable(portDeviceVecProp);

		IText * portProp = IndiHelperT::getText(portDeviceVecProp, "PORT");

		std::strcpy(portProp->text /*dest*/, inUsbDevicePort.c_str() /*src*/);

		mIndiClient->sendNewText(portDeviceVecProp);

	} catch (IndiExceptionT & exc) {
		throw DeviceConnectorExceptionT(
				"Unable to set device port. Details: "
						+ std::string(exc.what()));
	}
}
