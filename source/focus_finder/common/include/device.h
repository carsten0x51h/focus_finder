/*
 * device.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

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
