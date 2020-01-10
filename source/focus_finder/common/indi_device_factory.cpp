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

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "include/indi_device_factory.h"

#include "include/indi_driver_info.h"

#include "include/device.h"
#include "include/logging.h"
#include "include/throw_if.h"

#include "include/indi_client.h"
#include "include/indi_camera.h"
#include "include/indi_focus.h"
//#include "include/indi_filter.h"

IndiDeviceFactoryT::IndiDeviceFactoryT() {
	readIndiDriverInfo();
}

IndiDeviceFactoryT::~IndiDeviceFactoryT() {

}

void IndiDeviceFactoryT::readIndiDriverInfo() {

	LOG(debug) << "IndiDeviceFactoryT::readIndiDriverInfo..." << std::endl;

	// TODO: Do not hardcode filename... This one is for sure different for different OSs
	//       and installations. Check the kstars implementation of INDI - see ksutils.cpp
	//       for default paths and indidriver.cpp for loading of driver.xml.
	//       In kstars a file named Options.h is included everywhere which holds a
	//       method "Options::indiDriversDir()". This header file appears to be generated
	//       somehow at compiletime - at least is does not exist in the repo by default.
	IndiDriverInfoT driverInfo("/usr/share/indi/drivers.xml");

	// TODO / NOTE:
	// The code below is from indidriver.cpp (kstars). It shows that *_sk.xml files are skipped.
	// However, that means, in newer INDI versions, a skeleton file exists which contains all the
	// properties of the driver (and default values?). Question is, if this file should be read
	// manually or if INDI will do this automatically.
	// In addition, things have changed: Now there is one XML file (and sometimes a corresponding *_sk.xml
	// file) per driver. Therefore, the current implementation of kstars should be checked! 
	//
	// libindi 0.7.1: Skip skeleton files
        // if (fileInfo.fileName().endsWith(QLatin1String("_sk.xml")))
        //     continue;


	
	for (const auto & devGroup : driverInfo.devGroups()) {
		for (const auto & dev : devGroup.devices()) {
			std::string driverName = dev.driver().name();
			std::string deviceType = devGroup.name();

			LOG(debug) << "Driver: " << driverName << " --> "
					<< " DevicesType: " << deviceType << std::endl;

			mDeviceTypeMap.insert(std::make_pair(driverName, deviceType));
		}
	}
}

std::vector<std::string> IndiDeviceFactoryT::getDevicesByType(
		const std::string & deviceTypeName) const {

	auto deviceTypeFilter =
			[=](const auto & e) {return e.second == deviceTypeName;};

	std::vector < std::string > res;

	boost::copy(
			mDeviceTypeMap
					| boost::adaptors::filtered(std::ref(deviceTypeFilter))
					| boost::adaptors::map_keys, std::back_inserter(res));

	return res;
}

// TODO: return IndiDeviceTypeT?
std::string IndiDeviceFactoryT::getDeviceType(
		const std::string & deviceName) const {
	auto it = mDeviceTypeMap.find(deviceName);
	return (it != mDeviceTypeMap.end() ? it->second : "");
}

bool IndiDeviceFactoryT::isCamera(INDI::BaseDevice *dp) {
	return (strcmp("CCDs", getDeviceType(dp->getDeviceName()).c_str()) == 0);
}

bool IndiDeviceFactoryT::isFocus(INDI::BaseDevice *dp) {
	return (strcmp("Focusers", getDeviceType(dp->getDeviceName()).c_str()) == 0);
}

bool IndiDeviceFactoryT::isFilter(INDI::BaseDevice *dp) {
	return (strcmp("Filter Wheels", getDeviceType(dp->getDeviceName()).c_str())
			== 0);
}

std::shared_ptr<DeviceT> IndiDeviceFactoryT::createDevice(INDI::BaseDevice *dp,
		IndiClientT * indiClient) {

	THROW_IF(IndiDeviceFactory, dp == nullptr, "INDI::BaseDevice must not be 0.");
	THROW_IF(IndiDeviceFactory, indiClient == nullptr, "IndiClientT must not be 0.");

	std::shared_ptr<DeviceT> device;

	if (isCamera(dp)) {

		device = std::make_shared < IndiCameraT > (dp, indiClient);

	} else if (isFocus(dp)) {

		device = std::make_shared < IndiFocusT > (dp, indiClient);

	} else if (isFilter(dp)) {

		//device = std::make_shared<IndiFilterT>(dp, indiClient);
		//TODO
		device = nullptr;
	} else {

		LOG(warning) << "Unable to determine device type for device '"
				<< dp->getDeviceName() << "'." << std::endl;

		device = nullptr;
	}
	return device;
}
