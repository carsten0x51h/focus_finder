/*
 * dummy_focus.cpp
 *
 *  Created on: Apr 20, 2019
 *      Author: devnull
 */

#include <thread>
#include <atomic>
#include <chrono>

#include "include/dummy_device_connector.h"

using namespace std::chrono_literals;

DummyDeviceConnectorT::DummyDeviceConnectorT() :
	mConnectionState(DeviceConnectionStateT::DISCONNECTED),
	cancelConnectFlag(false),
	mDevicePort("/dev/ttyUSB0") {
	// Read connection status etc. from driver?
}

DummyDeviceConnectorT::~DummyDeviceConnectorT() {
}

void DummyDeviceConnectorT::connect() {

	// Start connection emulator thread...
	if (isConnected() || isConnecting()) {
		return;
	}

	LOG(debug)
	<< "DummyConnectorT - connect..." << std::endl;

	cancelConnectFlag = false;

	connectThread = std::thread(&DummyDeviceConnectorT::connectSimulation, this);
	connectThread.detach();
}

// Expected to be non-blocking
void DummyDeviceConnectorT::disconnect() {
	// Start disconnection emulator thread... (?)
	cancelConnectFlag = true;

	mConnectionState = DeviceConnectionStateT::DISCONNECTED;

	notifyDeviceDisconnected();
}

bool DummyDeviceConnectorT::isConnected() const {
	return (mConnectionState.load() == DeviceConnectionStateT::CONNECTED);
}

bool DummyDeviceConnectorT::isConnecting() const {
	return (mConnectionState.load() == DeviceConnectionStateT::CONNECTING);
}

bool DummyDeviceConnectorT::isDisconnected() const {
	return (mConnectionState.load() == DeviceConnectionStateT::DISCONNECTED);
}

bool DummyDeviceConnectorT::isDisconnecting() const {
	return (mConnectionState.load() == DeviceConnectionStateT::DISCONNECTING);
}

DeviceConnectionStateT::TypeE DummyDeviceConnectorT::getConnectionState() const {
	return mConnectionState;
}

bool DummyDeviceConnectorT::supportsDevicePort() const {
	return true; // TODO...
}

std::string DummyDeviceConnectorT::getDevicePort() const {
	return mDevicePort;
}

void DummyDeviceConnectorT::setDevicePort(const std::string & inDevicePort) {
	mDevicePort = inDevicePort;
}

/**
 * NOTE: Real hardware also needs to uses
 *
 * -notifyDeviceDisconnected()
 * -notifyDeviceConnectionTimeout()
 */
void DummyDeviceConnectorT::connectSimulation() {

	int simulationConnectTime = 2;

	mConnectionState = DeviceConnectionStateT::CONNECTING;

	notifyDeviceConnecting(); // Notify that device starts to connect...

	while (simulationConnectTime > 0) {
		LOG(debug)
		<< "Connecting...'" << simulationConnectTime << "'..."
				<< std::endl;

		std::this_thread::sleep_for(1s);
		--simulationConnectTime;

		if (cancelConnectFlag.load()) {
			notifyDeviceDisconnected();
			return;
		}
	}

	mConnectionState = DeviceConnectionStateT::CONNECTED;

	notifyDeviceConnected();
}
