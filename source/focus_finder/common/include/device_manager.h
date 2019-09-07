/*
 * device_manager.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */
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
