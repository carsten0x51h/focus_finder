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

#include "device_connector.h"
#include "device_type.h"

class DeviceT {
private:
	// We do not want device copies
	DeviceT(const DeviceT &);
	DeviceT & operator=(const DeviceT &);

protected:
	DeviceTypeT::TypeE mDeviceType;

public:
	static const std::string NONE;

	DeviceT() : mDeviceType(DeviceTypeT::_Count) {}

	virtual std::string getName() const = 0;
	virtual std::shared_ptr<DeviceConnectorT> getConnector() const = 0;
	virtual ~DeviceT() { };

	DeviceTypeT::TypeE getDeviceType() const { return mDeviceType; }
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEVICE_H_ */
