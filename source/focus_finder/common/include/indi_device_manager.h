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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_DEVICE_MANAGER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_DEVICE_MANAGER_H_

#include <map>
#include <memory>

#include "device.h"
#include "device_manager.h"
#include "exception.h"

#include "indi_client.h"

class IndiDeviceT;

DEF_Exception(IndiDevceManager);


class IndiDeviceManagerT : public DeviceManagerT {

private:
    typedef boost::signals2::signal<void(std::shared_ptr<DeviceT>)> DeviceAddedListenersT;
    DeviceAddedListenersT mDeviceAddedListeners;

    typedef boost::signals2::signal<void(std::shared_ptr<DeviceT>)> DeviceRemovedListenersT;
    DeviceRemovedListenersT mDeviceRemovedListeners;


    // INDI specific properties
    IndiClientT mIndiClient;

    boost::signals2::connection mRemoveDeviceConnection;
    boost::signals2::connection mNewDeviceConnection;
    boost::signals2::connection mNewMessageConnection;
    boost::signals2::connection mNewPropertyConnection;

    std::map<std::string, std::shared_ptr<IndiDeviceT> > mDeviceMap;
    mutable std::mutex mDeviceMapMutex;

    void addNewDevice(INDI::BaseDevice *dp);

protected:
    void newDevice(INDI::BaseDevice *dp);

    void newProperty(INDI::Property *property);

    void removeDevice(INDI::BaseDevice *dp);

    static void newMessage(INDI::BaseDevice *dp, int messageID);

    void notifyDeviceAdded(std::shared_ptr<DeviceT> device) { mDeviceAddedListeners(device); };

    void notifyDeviceRemoved(std::shared_ptr<DeviceT> device) { mDeviceRemovedListeners(device); };

public:
    // TODO: Instead of specific properties maybe bettwer use one key-value "Properties" object...
    [[maybe_unused]] static const std::string sDefaultIndiHostname;
    [[maybe_unused]] static const int sDefaultIndiPort;


    IndiDeviceManagerT();

    virtual ~IndiDeviceManagerT();

    DeviceManagerTypeT::TypeE getDeviceManagerType() const;

    bool isReady() const;

    void configure(const boost::property_tree::ptree & deviceManagerConfig);


    std::shared_ptr<DeviceT> getDevice(const std::string &deviceName);

    std::vector<std::shared_ptr<DeviceT> > getDevices(DeviceInterfaceTypeT::TypeE interfaceType);

    // TODO: Make const...
    std::vector<std::string> getDeviceNames(DeviceInterfaceTypeT::TypeE interfaceType);


    boost::signals2::connection registerDeviceAddedListener(const DeviceAddedListenersT::slot_type &inCallBack) {
        return mDeviceAddedListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterDeviceAddedListener(const T &inCallBack) {
        inCallBack.disconnect();
    }


    boost::signals2::connection registerDeviceRemovedListener(const DeviceRemovedListenersT::slot_type &inCallBack) {
        return mDeviceRemovedListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterDeviceRemovedListener(const T &inCallBack) {
        inCallBack.disconnect();
    }



    //	std::vector<std::string> getCameraInterfaceList() const;
//	std::shared_ptr<CameraInterfaceT> getCameraInterface(const std::string & cameraName) const;
//
//	std::vector<std::string> getFocusInterfaceList() const;
//	std::shared_ptr<FocusInterfaceT> getFocusInterface(const std::string & focusName) const;
//
//	std::vector<std::string> getFilterInterfaceList() const;
//	std::shared_ptr<FilterInterfaceT> getFilterInterface(const std::string & filterName) const;

    // INDI specific settings
    // TODO: Maybe introduce a generic setup(Properties props), setProperty(), getProperty() functions instead of specific functions...
    void setHostname(const std::string &hostname);

    std::string getHostname() const;

    void setPort(unsigned int port);

    unsigned int getPort() const;

    IndiClientT &getIndiClient();
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_DEVICE_MANAGER_H_ */
