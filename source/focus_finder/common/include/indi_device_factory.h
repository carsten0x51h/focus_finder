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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INDI_DEVICE_FACTORY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INDI_DEVICE_FACTORY_H_

#include <memory>
#include <map>

// INDI
#include "basedevice.h"

#include "exception.h"

DEF_Exception(IndiDeviceFactory);

class IndiClientT;
class DeviceT;



class IndiDeviceFactoryT {
private:
	bool isCamera(INDI::BaseDevice *dp);
	bool isFocus(INDI::BaseDevice *dp);
	bool isFilter(INDI::BaseDevice *dp);

	void readIndiDriverInfo();

	// We do not want device copies
	IndiDeviceFactoryT(const IndiDeviceFactoryT &);
	IndiDeviceFactoryT & operator=(const IndiDeviceFactoryT &);

    std::map<std::string, std::string /*TODO: replace by DeviceTypeT? or IndiDeviceTypeT?*/> mDeviceTypeMap;

public:
	IndiDeviceFactoryT();
	~IndiDeviceFactoryT();

	std::shared_ptr<DeviceT> createDevice(INDI::BaseDevice *dp, IndiClientT * indiClient);

	// TODO: Maybe introduce DeviceTypeT (independent from INDI) -> getDeviceType()...
	std::string getDeviceType(const std::string & deviceName) const;

	// TODO: Maybe introduce DeviceTypeT (independent from INDI) -> getDeviceType()...
	std::vector<std::string> getDevicesByType(const std::string & deviceTypeName) const;

};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INDI_DEVICE_FACTORY_H_ */


