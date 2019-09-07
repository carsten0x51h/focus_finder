/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DUMMY_DEVICE_MANAGER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DUMMY_DEVICE_MANAGER_H_

#include "device_manager.h"

class DummyDeviceManagerT : public DeviceManagerT {

public:
	DummyDeviceManagerT();
	virtual ~DummyDeviceManagerT();

	// set dev profile... -> TODO: sep. class for this?
	// get dev profile... -> TODO: sep. class for this?

	std::shared_ptr<DeviceT> getDevice(const std::string & deviceName) const;

	std::vector<std::string> getCameraList() const;
	std::shared_ptr<CameraT> getCamera(const std::string & cameraName) const;

	std::vector<std::string> getFocusList() const;
	std::shared_ptr<FocusT> getFocus(const std::string & focusName) const;

	std::vector<std::string> getFilterList() const;
	std::shared_ptr<FilterT> getFilter(const std::string & filterName) const;

private:
	// TODO/HACK
	std::shared_ptr<CameraT> mCamera1;
	std::shared_ptr<CameraT> mCamera2;
	std::shared_ptr<CameraT> mCamera3;
    std::shared_ptr<FocusT> mFocus1;
    std::shared_ptr<FocusT> mFocus2;
    std::shared_ptr<FocusT> mFocus3;
    std::shared_ptr<FilterT> mFilter1;
    std::shared_ptr<FilterT> mFilter2;
    std::shared_ptr<FilterT> mFilter3;
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DUMMY_DEVICE_MANAGER_H_ */
