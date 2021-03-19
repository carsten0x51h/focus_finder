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

#include <iostream>

#include "include/dummy_device_manager.h"

#include "include/logging.h"

#include "include/camera_interface.h"
#include "include/focus_interface.h"
#include "include/filter_interface.h"
#include "include/cooler.h"

#include "include/dummy_camera.h"
#include "include/dummy_focus.h"
#include "include/dummy_filter.h"

DummyDeviceManagerT::DummyDeviceManagerT() {
	std::cerr << "DummyDeviceManagerT::DummyDeviceManagerT()..." << std::endl;

	// TODO: Move somewhere else.... and do not harcode... use profile....
	mCamera1 = std::make_shared<DummyCameraT>();
	mCamera2 = std::make_shared<DummyCameraT>();
	mCamera3 = std::make_shared<DummyCameraT>();

	mFocus1 = std::make_shared<DummyFocusT>();
	mFocus2 = std::make_shared<DummyFocusT>();
	mFocus3 = std::make_shared<DummyFocusT>();

	mFilter1 = std::make_shared<DummyFilterT>();
	mFilter2 = std::make_shared<DummyFilterT>();
	mFilter3 = std::make_shared<DummyFilterT>();
}

DummyDeviceManagerT::~DummyDeviceManagerT() {

}

bool DummyDeviceManagerT::isReady() {
  return true;
}

DeviceManagerTypeT::TypeE DummyDeviceManagerT::getDeviceManagerType() const {
  return DeviceManagerTypeT::DUMMY;
}

std::shared_ptr<DeviceInterfaceT> DummyDeviceManagerT::getDeviceInterface(const std::string & deviceName) const {
	LOG(debug) << "DummyDeviceManagerT::getDeviceInterface - deviceName: " << deviceName << std::endl;
	//list.push_back(std::make_shared<Test>());

	// HACK!!!
	if (deviceName == "Focus 1")
		return mFocus1;
	else if (deviceName == "Focus 2")
		return mFocus2;
	else if (deviceName == "Focus 3")
		return mFocus3;

	else if (deviceName == "Filter 1")
		return mFilter1;
	else if (deviceName == "Filter 2")
		return mFilter2;
	else if (deviceName == "Filter 3")
		return mFilter3;

	else if (deviceName == "Camera 1")
		return mCamera1;
	else if (deviceName == "Camera 2")
		return mCamera2;
	else if (deviceName == "Camera 3")
		return mCamera3;

	else
		return nullptr;
}

std::vector<std::string> DummyDeviceManagerT::getCameraInterfaceList() const {
	std::vector<std::string> cameras = {"Camera 1", "Camera 2", "Camera 3"};
	return cameras;
}

std::shared_ptr<CameraInterfaceT> DummyDeviceManagerT::getCameraInterface(const std::string & cameraName) const {
	LOG(debug) << "DummyDeviceManagerT::getCameraInterface - cameraName: " << cameraName << std::endl;
	//list.push_back(std::make_shared<Test>());

	// TODO / HACK
	return std::static_pointer_cast<CameraInterfaceT>(getDeviceInterface(cameraName));
}


std::vector<std::string> DummyDeviceManagerT::getFocusInterfaceList() const {
	std::vector<std::string> focuses = {"Focus 1", "Focus 2", "Focus 3"};
	return focuses;
}

std::shared_ptr<FocusInterfaceT> DummyDeviceManagerT::getFocusInterface(const std::string & focusName) const {
	LOG(debug) << "DummyDeviceManagerT::getFocusInterface - focusName: " << focusName << std::endl;
	//list.push_back(std::make_shared<Test>());

	// TODO / HACK
	return std::static_pointer_cast<FocusInterfaceT>(getDeviceInterface(focusName));
}


std::vector<std::string> DummyDeviceManagerT::getFilterInterfaceList() const {
	std::vector<std::string> filters = { "Filter 1", "Filter 2", "Filter 3" };
	return filters;
}

std::shared_ptr<FilterInterfaceT> DummyDeviceManagerT::getFilterInterface(const std::string & filterName) const {
	LOG(debug) << "DummyDeviceManagerT::getFilterInterface - filterName: " << filterName << std::endl;
	//list.push_back(std::make_shared<Test>());

	// TODO / HACK
	return std::static_pointer_cast<FilterInterfaceT>(getDeviceInterface(filterName));
}

// TODO: Same for Filter and Cooler...
