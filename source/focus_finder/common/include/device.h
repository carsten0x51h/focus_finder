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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEVICE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEVICE_H_

#include <memory>
#include <set>

#include <boost/signals2.hpp>

#include "exception.h"
#include "device_interface.h"
#include "device_interface_type.h"
#include "device_connection_state.h"

DEF_Exception(Device);

class DeviceInterfaceT;
class CameraInterfaceT;

/**
 * TODO: Check mIsAvailable is all the functions.... maybe set the flag atomic as well...
 */
class DeviceT {

private:
	// We do not want device copies
	DeviceT(const DeviceT &);
	DeviceT & operator=(const DeviceT &);

	/**
	 * This flag is true if the corresponding part on the serverside is available (common case).
	 * However, if a device gets removed for some reason on the server side, this flag will turn false.
	 * It will turn true again, when the device becomes available again. In this case also a new
	 * base device handle will be set.
	 */
	bool mIsAvailable;

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
	DeviceT();
    virtual ~DeviceT();

	virtual std::string getName() const = 0;


	/**
	 * A bit-mask would of course be more efficient but as interface
	 * function a set of the supported interface types is more intuitive
	 * for less experienced developers.
	 *
	 * TODO: If it turns out to be unpractical, convert this to a uint16_t as bit-mask...
	 *
	 * @return
	 */
	virtual std::set<DeviceInterfaceTypeT::TypeE> getSupportedInferfaces() const = 0;


	/**
	 * Uses getSupportedInferfaceTypes() to easily check if a given interface type is supported
	 * by the device.
	 *
	 * @return
	 */
	bool isInterfaceSupported(DeviceInterfaceTypeT::TypeE) const;


	/**
	 *
	 */
	virtual std::shared_ptr<DeviceInterfaceT> getInterface(DeviceInterfaceTypeT::TypeE interfaceType) = 0;


	/**
	 * Conenience function to get the camera interface.
	 *
	 * @return
	 */
    std::shared_ptr<CameraInterfaceT> getCameraInterface();

	/**
	 * DeviceInterfaceT has no state. It is just a wrapper / facade to make access
	 * to the device interface more comfortable.
	 * TODO / WRONG! DeviceInterfaces can have a state - e.g. IndiCameraInterfaceT has an additional state for counting the number of images to record
	 * --> Probably this is not a good idea. Should additional states be removed from a wrapper?
	 * --> Depending on that retuen by value or a shred_ptr... I guess a PTR is required anyway!!!!!
	 *
	 * Throws if requested interface type is not supported.
	 *
	 * @param interfaceType
	 * @return
	 */
// TODO............
	//	virtual DeviceInterfaceT getInterface(DeviceInterfaceTypeT::TypeE interfaceType) = 0;
//
//	OR just
//
//	std::shared<CameraInterfaceT> getCameraInterface() const; Returns instance of shared_ptr<IndiCameraInterface>...
//	OR: CameraInterfaceT is returned by value and just Wraps an IndiCameraInterfaceHanderT....
//
//	FocusInterfaceT getFocusInterface() const;
//	FilterInterfaceT getFilterInterface() const;


    // From device_manager...
//    virtual std::vector<std::string> getCameraInterfaceList() const = 0;
//    virtual std::shared_ptr<CameraInterfaceT> getCameraInterface(const std::string & cameraName) const = 0;
//
//    virtual std::vector<std::string> getFocusInterfaceList() const = 0;
//    virtual std::shared_ptr<FocusInterfaceT> getFocusInterface(const std::string & focusName) const = 0;
//
//    virtual std::vector<std::string> getFilterInterfaceList() const = 0;
//    virtual std::shared_ptr<FilterInterfaceT> getFilterInterface(const std::string & filterName) const = 0;




    virtual void connect() = 0;     // Expected to be non-blocking
    virtual void disconnect() = 0;  // Expected to be non-blocking

    virtual bool isConnected() const = 0;
    virtual bool isConnecting() const = 0;
    virtual bool isDisconnected() const = 0;
    virtual bool isDisconnecting() const = 0;

    bool isAvailable() const;
    void setAvailable(bool isAvailable);


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


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEVICE_H_ */
