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

class CameraT;
class FocusT;
class CoolerT;
class FilterT;
class DeviceT;


class DeviceManagerT {
public:
	virtual std::shared_ptr<DeviceT> getDevice(const std::string & deviceName) const = 0;

	virtual std::vector<std::string> getCameraList() const = 0;
	virtual std::shared_ptr<CameraT> getCamera(const std::string & cameraName) const = 0;

	virtual std::vector<std::string> getFocusList() const = 0;
	virtual std::shared_ptr<FocusT> getFocus(const std::string & focusName) const = 0;

	virtual std::vector<std::string> getFilterList() const = 0;
	virtual std::shared_ptr<FilterT> getFilter(const std::string & filterName) const = 0;


//	signals2::connection registerExposureCycleFinishedListener(const ExposureCycleFinishedListenersT::slot_type & inCallBack) {
//		return mExposureCycleFinishedListeners.connect(inCallBack);
//	}
//	template <class T> void unregisterExposureFinishedListener(const T & inCallBack) {
//		//mExposureCycleFinishedListeners.disconnect(inCallBack);
//  	inCallBack.disconnect();
//	}
//
//protected:
//	void notifyExposureCycleFinished(RectT<unsigned int> roiRect, std::shared_ptr<const ImageT> resultImage, bool lastExposure) { mExposureCycleFinishedListeners(roiRect, resultImage, lastExposure); }
//
//private:
//	typedef signals2::signal<void (DeviceTypeT::TypeE, )> DeviceConnectionChangedListenersT;
//	DeviceConnectionChangedListenersT mDeviceConnectionChangedListeners;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEVICE_MANAGER_H_ */
