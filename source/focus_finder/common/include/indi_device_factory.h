/*
 * indi_camera.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

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


