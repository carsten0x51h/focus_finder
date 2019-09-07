/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_DEVICE_MANAGER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_DEVICE_MANAGER_H_

#include <map>
#include <memory>

#include "device.h"
#include "device_manager.h"

#include "indi_client.h"
#include "indi_device_factory.h"

class IndiDeviceManagerT : public DeviceManagerT {

public:
	static const std::string sDefaultIndiHostname;
	static int sDefaultIndiPort;


	IndiDeviceManagerT();
	virtual ~IndiDeviceManagerT();

	std::shared_ptr<DeviceT> getDevice(const std::string & deviceName) const;

	std::vector<std::string> getCameraList() const;
	std::shared_ptr<CameraT> getCamera(const std::string & cameraName) const;

	std::vector<std::string> getFocusList() const;
	std::shared_ptr<FocusT> getFocus(const std::string & focusName) const;

	std::vector<std::string> getFilterList() const;
	std::shared_ptr<FilterT> getFilter(const std::string & filterName) const;

	// INDI specific settings
	void setHostname(const std::string & hostname);
	std::string getHostname() const;

	void setPort(unsigned int port);
	unsigned int getPort() const;

	// TODO: Need connect/disconect logic to server (+error handling) and listners (events)
	//void connectServer();
	bool connectServer();
	//bool disconnectServer();

protected:
	void newDevice(INDI::BaseDevice *dp);
	void removeDevice(INDI::BaseDevice *dp);
	void newMessage(INDI::BaseDevice *dp, int messageID);

private:
	std::vector<std::string> getDeviceList(DeviceTypeT::TypeE deviceType) const;

	IndiDeviceFactoryT mIndiDeviceFactory;

    // INDI specific properties
    IndiClientT mIndiClient;

    boost::signals2::connection mRemoveDeviceConnection;
    boost::signals2::connection mNewDeviceConnection;
    boost::signals2::connection mNewMessageConnection;

    std::map<std::string, std::shared_ptr<DeviceT> > mDeviceMap;
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_DEVICE_MANAGER_H_ */
