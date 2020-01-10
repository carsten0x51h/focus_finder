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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEVICE_CONNECTOR_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEVICE_CONNECTOR_H_

#include <boost/signals2.hpp>

#include "exception.h"

DEF_Exception(DeviceConnector);

struct DeviceConnectionStateT {
	typedef enum {
		DISCONNECTED,
		CONNECTING,
		CONNECTED,
		DISCONNECTING,
		_Count
	} TypeE;

	static const char * asStr(const TypeE & inType) {
		switch (inType) {
			case DISCONNECTED: return "DISCONNECTED";
			case CONNECTING: return "CONNECTING";
			case CONNECTED: return "CONNECTED";
			case DISCONNECTING: return "DISCONNECTING";
			default: return "<?>";
		}
	}
};


class DeviceConnectorT {
private:
	typedef boost::signals2::signal<void (void)> DeviceConnectingListenersT;
	typedef boost::signals2::signal<void (void)> DeviceConnectedListenersT;
	typedef boost::signals2::signal<void (void)> DeviceDisconnectingListenersT;
	typedef boost::signals2::signal<void (void)> DeviceDisconnectedListenersT;
	//typedef boost::signals2::signal<void (void)> DeviceConnectCancelledListenersT;
	typedef boost::signals2::signal<void (void)> DeviceConnectionTimeoutListenersT;

	DeviceConnectedListenersT mDeviceConnectedListeners;
	DeviceConnectingListenersT mDeviceConnectingListeners;
	DeviceDisconnectedListenersT mDeviceDisconnectedListeners;
	DeviceDisconnectingListenersT mDeviceDisconnectingListeners;
	//DeviceConnectCancelledListenersT mDeviceConnectCancelledListeners;
	DeviceConnectionTimeoutListenersT mDeviceConnectionTimeoutListeners;

public:
	virtual void connect() = 0;     // Expected to be non-blocking
	virtual void disconnect() = 0;  // Expected to be non-blocking

	virtual bool isConnected() const = 0;
	virtual bool isConnecting() const = 0;
	virtual bool isDisconnected() const = 0;
	virtual bool isDisconnecting() const = 0;

	virtual DeviceConnectionStateT::TypeE getConnectionState() const = 0;

	// Register on events
	boost::signals2::connection registerDeviceConnectedListener(const DeviceConnectedListenersT::slot_type & inCallBack) {
		return mDeviceConnectedListeners.connect(inCallBack);
	}
	template <class T> void unregisterDeviceConnectedListener(const T & inCallBack) {
		//mDeviceConnectedListeners.disconnect(inCallBack);
		inCallBack.disconnect();
	}

	boost::signals2::connection registerDeviceConnectingListener(const DeviceConnectingListenersT::slot_type & inCallBack) {
		return mDeviceConnectingListeners.connect(inCallBack);
	}
	template <class T> void unregisterDeviceConnectingListener(const T & inCallBack) {
		//mDeviceConnectingListeners.disconnect(inCallBack);
		inCallBack.disconnect();
	}

	boost::signals2::connection registerDeviceDisconnectingListener(const DeviceDisconnectingListenersT::slot_type & inCallBack) {
		return mDeviceDisconnectingListeners.connect(inCallBack);
	}
	template <class T> void unregisterDeviceDisconnectingListener(const T & inCallBack) {
		//mDeviceConnectingListeners.disconnect(inCallBack);
		inCallBack.disconnect();
	}

//	boost::signals2::connection registerDeviceConnectCancelledListener(const DeviceConnectCancelledListenersT::slot_type & inCallBack) {
//		return mDeviceConnectCancelledListeners.connect(inCallBack);
//	}
//	template <class T> void unregisterDeviceConnectCancelledListener(const T & inCallBack) {
//		//mDeviceConnectCancelledListeners.disconnect(inCallBack);
//		inCallBack.disconnect();
//	}

	boost::signals2::connection registerDeviceDisconnectedListener(const DeviceDisconnectedListenersT::slot_type & inCallBack) {
		return mDeviceDisconnectedListeners.connect(inCallBack);
	}
	template <class T> void unregisterDeviceDisconnectedListener(const T & inCallBack) {
		//mDeviceDisconnectedListeners.disconnect(inCallBack);
		inCallBack.disconnect();
	}

	boost::signals2::connection registerConnectionTimeoutListener(const DeviceConnectionTimeoutListenersT::slot_type & inCallBack) {
		return mDeviceConnectionTimeoutListeners.connect(inCallBack);
	}
	template <class T> void unregisterConnectionTimeoutListener(const T & inCallBack) {
		//mDeviceConnectionTimeoutListeners.disconnect(inCallBack);
		inCallBack.disconnect();
	}

protected:
	// To be used by startExposure, cancelExposure and actual device implementation.
	void notifyDeviceConnected() { mDeviceConnectedListeners(); };
	void notifyDeviceConnecting() { mDeviceConnectingListeners(); };
	void notifyDeviceDisconnected() { mDeviceDisconnectedListeners(); };
	void notifyDeviceDisconnecting() { mDeviceDisconnectingListeners(); };
	//void notifyDeviceConnectCancelled() { mDeviceConnectCancelledListeners(); }
	void notifyDeviceConnectionTimeout() { mDeviceConnectionTimeoutListeners(); }
};



#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEVICE_CONNECTOR_H_ */
