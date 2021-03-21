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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEVICE_MANAGER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEVICE_MANAGER_H_


#include <vector>
#include <memory>
#include <boost/signals2.hpp>

#include "device_manager_type.h"
#include "device.h"

class DeviceManagerT {
private:

public:
    virtual DeviceManagerTypeT::TypeE getDeviceManagerType() const = 0;

    virtual bool isReady() const = 0;

    /**
     * Get device object ptr by name.
     *
     * @param deviceName Name of the device
     * @return Returns nullptr if not found.
     */
    virtual std::shared_ptr<DeviceT> getDevice(const std::string &deviceName) = 0;

    /**
     * Get list of device object ptrs by interface type.
     *
     * @param interfaceType Type of the device interface
     * @return Returns empty vector if no devices with given interface type found.
     */
    virtual std::vector<std::shared_ptr<DeviceT> > getDevices(DeviceInterfaceTypeT::TypeE interfaceType) = 0;

    // Convenience functions...
    /**
     * Get list of all device names for a given interface type.
     *
     * @param interfaceType
     * @return Returns list of all devices (names) which support the given interface type.
     */
    virtual std::vector<std::string> getDeviceNames(DeviceInterfaceTypeT::TypeE interfaceType) = 0;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEVICE_MANAGER_H_ */
