/*
 * indi_camera.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INDI_USB_DEVICE_CONNECTOR_H_
#define SOURCE_FOCUS_FINDER_COMMON_INDI_USB_DEVICE_CONNECTOR_H_

#include <string>
#include <atomic>
#include <thread>

#include "logging.h"
#include "device_connector.h"
#include "indi_client.h"

// libindi
#include "basedevice.h"

class IndiUsbDeviceConnectorT : public DeviceConnectorT {

public:
	IndiUsbDeviceConnectorT(INDI::BaseDevice *dp, IndiClientT * indiClient);
	virtual ~IndiUsbDeviceConnectorT();

	void connect();
	void disconnect();
	bool isConnected() const;
	bool isConnecting() const;
	bool isDisconnected() const;
	bool isDisconnecting() const;

	DeviceConnectionStateT::TypeE getConnectionState() const;

	// USB
	std::string getUsbDevicePort() const; // e.g. /dev/ttyUSB0 - TODO: How is this handled in Windows?
	void setUsbDevicePort(const std::string & usbDevicePort);

private:
	DeviceConnectionStateT::TypeE getConnectionStateInternal() const;
	void connectInternal();
	void disconnectInternal();


	INDI::BaseDevice * mIndiBaseDevice;
	IndiClientT * mIndiClient;

	std::atomic<bool> mCancelConnectFlag;
	std::thread mConnectThread;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INDI_DEVICE_CONNECTOR_H_ */
