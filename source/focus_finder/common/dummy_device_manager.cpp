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

#include "include/camera.h"
#include "include/focus.h"
#include "include/filter.h"
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

std::shared_ptr<DeviceT> DummyDeviceManagerT::getDevice(const std::string & deviceName) const {
	LOG(debug) << "DummyDeviceManagerT::getDevice - deviceName: " << deviceName << std::endl;
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

std::vector<std::string> DummyDeviceManagerT::getCameraList() const {
	std::vector<std::string> cameras = {"Camera 1", "Camera 2", "Camera 3"};
	return cameras;
}

std::shared_ptr<CameraT> DummyDeviceManagerT::getCamera(const std::string & cameraName) const {
	LOG(debug) << "DummyDeviceManagerT::getCamera - cameraName: " << cameraName << std::endl;
	//list.push_back(std::make_shared<Test>());

	// TODO / HACK
	return std::static_pointer_cast<CameraT>(getDevice(cameraName));
}


std::vector<std::string> DummyDeviceManagerT::getFocusList() const {
	std::vector<std::string> focuses = {"Focus 1", "Focus 2", "Focus 3"};
	return focuses;
}

std::shared_ptr<FocusT> DummyDeviceManagerT::getFocus(const std::string & focusName) const {
	LOG(debug) << "DummyDeviceManagerT::getFocus - focusName: " << focusName << std::endl;
	//list.push_back(std::make_shared<Test>());

	// TODO / HACK
	return std::static_pointer_cast<FocusT>(getDevice(focusName));
}


std::vector<std::string> DummyDeviceManagerT::getFilterList() const {
	std::vector<std::string> filters = { "Filter 1", "Filter 2", "Filter 3" };
	return filters;
}

std::shared_ptr<FilterT> DummyDeviceManagerT::getFilter(const std::string & filterName) const {
	LOG(debug) << "DummyDeviceManagerT::getFilter - filterName: " << filterName << std::endl;
	//list.push_back(std::make_shared<Test>());

	// TODO / HACK
	return std::static_pointer_cast<FilterT>(getDevice(filterName));
}

// TODO: Same for Filter and Cooler...
