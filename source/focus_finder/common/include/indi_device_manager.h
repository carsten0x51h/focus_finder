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

#include "indi_client.h"
#include "indi_device_factory.h"

class IndiDeviceManagerT : public DeviceManagerT {

public:
	static const std::string sDefaultIndiHostname;
	static const int sDefaultIndiPort;


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
