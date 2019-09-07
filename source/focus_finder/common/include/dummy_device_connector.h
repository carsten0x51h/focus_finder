/*
 * indi_camera.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_DUMMY_DEVICE_CONNECTOR_H_
#define SOURCE_FOCUS_FINDER_COMMON_DUMMY_DEVICE_CONNECTOR_H_

#include <thread>
#include <chrono>
#include <string>

#include "logging.h"
#include "device_connector.h"

class DummyDeviceConnectorT : public DeviceConnectorT {

public:
	DummyDeviceConnectorT();
	virtual ~DummyDeviceConnectorT();

	void connect();
	void disconnect();

	bool isConnected() const;
	bool isConnecting() const;
	bool isDisconnected() const;
	bool isDisconnecting() const;


	DeviceConnectionStateT::TypeE getConnectionState() const;


	bool supportsDevicePort() const;
	std::string getDevicePort() const;
	void setDevicePort(const std::string & inDevicePort);

private:
	void connectSimulation();

	std::thread connectThread;
	std::atomic<DeviceConnectionStateT::TypeE> mConnectionState;
	std::atomic<bool> cancelConnectFlag;

	std::string mDevicePort;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_DUMMY_DEVICE_CONNECTOR_H_ */
