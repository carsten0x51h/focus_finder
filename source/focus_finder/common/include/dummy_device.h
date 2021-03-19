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

#ifndef SOURCE_FOCUS_FINDER_COMMON_DUMMY_DEVICE_CONNECTOR_H_
#define SOURCE_FOCUS_FINDER_COMMON_DUMMY_DEVICE_CONNECTOR_H_

#include <thread>
#include <chrono>
#include <string>
#include <set>

#include "logging.h"
#include "device.h"
#include "device_connection_state.h"

class DummyDeviceT : public DeviceT {

public:
	DummyDeviceT();
	virtual ~DummyDeviceT();

	void connect();
	void disconnect();

	bool isConnected() const;
	bool isConnecting() const;
	bool isDisconnected() const;
	bool isDisconnecting() const;


	DeviceConnectionStateT::TypeE getConnectionState() const;

    std::set<DeviceInterfaceTypeT::TypeE> getSupportedInferfaces() const;

private:
	void connectSimulation();

	std::thread connectThread;
	std::atomic<DeviceConnectionStateT::TypeE> mConnectionState;
	std::atomic<bool> cancelConnectFlag;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_DUMMY_DEVICE_CONNECTOR_H_ */
