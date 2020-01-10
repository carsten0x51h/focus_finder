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
