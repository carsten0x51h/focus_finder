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

#include <thread>
//#include <atomic>
#include <chrono>

#include "include/dummy_device.h"

using namespace std::chrono_literals;

DummyDeviceT::DummyDeviceT() :
	mConnectionState(DeviceConnectionStateT::DISCONNECTED),
	cancelConnectFlag(false)
{
	// Read connection status etc. from driver?
}

DummyDeviceT::~DummyDeviceT() {
}

void DummyDeviceT::connect() {

	// Start connection emulator thread...
	if (isConnected() || isConnecting()) {
		return;
	}

	LOG(debug)
	<< "DummyConnectorT - connect..." << std::endl;

	cancelConnectFlag = false;

	connectThread = std::thread(&DummyDeviceT::connectSimulation, this);
	connectThread.detach();
}

// Expected to be non-blocking
void DummyDeviceT::disconnect() {
	// Start disconnection emulator thread... (?)
	cancelConnectFlag = true;

	mConnectionState = DeviceConnectionStateT::DISCONNECTED;

	notifyDeviceDisconnected();
}

bool DummyDeviceT::isConnected() const {
	return (mConnectionState.load() == DeviceConnectionStateT::CONNECTED);
}

bool DummyDeviceT::isConnecting() const {
	return (mConnectionState.load() == DeviceConnectionStateT::CONNECTING);
}

bool DummyDeviceT::isDisconnected() const {
	return (mConnectionState.load() == DeviceConnectionStateT::DISCONNECTED);
}

bool DummyDeviceT::isDisconnecting() const {
	return (mConnectionState.load() == DeviceConnectionStateT::DISCONNECTING);
}

DeviceConnectionStateT::TypeE DummyDeviceT::getConnectionState() const {
	return mConnectionState;
}

/**
 * NOTE: Real hardware also needs to uses
 *
 * -notifyDeviceDisconnected()
 * -notifyDeviceConnectionTimeout()
 */
void DummyDeviceT::connectSimulation() {

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

std::set<DeviceInterfaceTypeT::TypeE> DummyDeviceT::getSupportedInferfaces() const {
    // TODO: Implement...
    return std::set<DeviceInterfaceTypeT::TypeE>();
}
