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
	//       In kstars a file named Options.h is included everywhere which alos holds a
	//       method "Options::indiDriversDir()". This header file appears to be generated
	//       somehow at compiletime - at least is does not exist in the repo by default.
	IndiDriverInfoT driverInfo("/usr/share/indi/drivers.xml");

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
