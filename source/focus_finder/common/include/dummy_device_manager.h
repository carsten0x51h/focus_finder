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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DUMMY_DEVICE_MANAGER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DUMMY_DEVICE_MANAGER_H_

#include "device_manager.h"

class DummyDeviceManagerT : public DeviceManagerT {

public:
  DummyDeviceManagerT();
  virtual ~DummyDeviceManagerT();

  // set dev profile... -> TODO: sep. class for this?
  // get dev profile... -> TODO: sep. class for this?

  bool isReady();
  
  DeviceManagerTypeT::TypeE getDeviceManagerType() const;

  std::shared_ptr<DeviceInterfaceT> getDeviceInterface(const std::string & deviceName) const;

  std::vector<std::string> getCameraInterfaceList() const;
  std::shared_ptr<CameraInterfaceT> getCameraInterface(const std::string & cameraName) const;

  std::vector<std::string> getFocusInterfaceList() const;
  std::shared_ptr<FocusInterfaceT> getFocusInterface(const std::string & focusName) const;

  std::vector<std::string> getFilterInterfaceList() const;
  std::shared_ptr<FilterInterfaceT> getFilterInterface(const std::string & filterName) const;

private:
  // TODO/HACK
  std::shared_ptr<CameraInterfaceT> mCamera1;
  std::shared_ptr<CameraInterfaceT> mCamera2;
  std::shared_ptr<CameraInterfaceT> mCamera3;
  std::shared_ptr<FocusInterfaceT> mFocus1;
  std::shared_ptr<FocusInterfaceT> mFocus2;
  std::shared_ptr<FocusInterfaceT> mFocus3;
  std::shared_ptr<FilterInterfaceT> mFilter1;
  std::shared_ptr<FilterInterfaceT> mFilter2;
  std::shared_ptr<FilterInterfaceT> mFilter3;
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DUMMY_DEVICE_MANAGER_H_ */
