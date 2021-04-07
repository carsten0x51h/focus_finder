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

#ifndef SOURCE_FOCUS_FINDER_COMMON_FOCUS_FINDER_LOGIC_H_
#define SOURCE_FOCUS_FINDER_COMMON_FOCUS_FINDER_LOGIC_H_ SOURCE_FOCUS_FINDER_COMMON_FOCUS_FINDER_LOGIC_H_

#include <memory>

#include <boost/signals2.hpp>

#include "star_info.h"
#include "image.h"
#include "rect.h"
#include "frame.h"
#include "size.h"
#include "task_executor.h"

class CameraInterfaceT;

class FocusInterfaceT;

class FilterInterfaceT;

class DeviceManagerT;

class ProfileManagerT;

class MapperFunctionT;

class ImageConverter16To8T;

class FocusFinderT;

class FocusCurveRecorderLogicT;

class FoFiConfigManagerT;

class FocusFinderLogicT {
public:
    FocusFinderLogicT();

    ~FocusFinderLogicT();

    std::shared_ptr<CameraInterfaceT> getCurrentCamera();

    std::shared_ptr<FocusInterfaceT> getCurrentFocus();

    std::shared_ptr<FilterInterfaceT> getCurrentFilter();

    [[nodiscard]] std::shared_ptr<DeviceManagerT> getDeviceManager() const;

    [[nodiscard]] std::shared_ptr<ProfileManagerT> getProfileManager() const;

    void setSelectedRoi(const RectT<unsigned int> &roi);

    [[nodiscard]] RectT<unsigned int> getSelectedRoi() const;

    void clearSelectedRoi();

    std::optional<PointT<float> > findFocusStar(const PointT<float> &poi);

    [[nodiscard]] const FrameT &getLastFrame() const;


    [[nodiscard]] std::vector<std::string> getMapperFunctionNames() const;

    [[nodiscard]] std::shared_ptr<MapperFunctionT>
    getMapperFunctionByName(const std::string &mapperFunctionName) const;

    [[nodiscard]] std::shared_ptr<ImageConverter16To8T> getImageConverter16To8() const;

    [[nodiscard]] std::shared_ptr<TaskExecutorT<FocusFinderT> > getFocusFinderExecutor() const;

    [[nodiscard]] std::optional<PointT<float> > getLastFocusStarPos() const;

    std::shared_ptr<FocusCurveRecorderLogicT> getFocusCurveRecorderLogic();


    static void init();

    static void close();

    /**
     * Example usage:
     *
     * FocusFinderLogicT::get()->getProfileManager()...
     */
    static FocusFinderLogicT *get();

private:
    static FocusFinderLogicT *sSelf;

    [[nodiscard]] static size_t calcNumStarsInRegion(const ImageT &inImg) ;

    void initImageMappers();

    void updateProfile();

    // TODO... remove?
    std::shared_ptr<DeviceManagerT> mDeviceManager;

    // TODO: Not yet sure, if sep. of DeviceManager and ProfileManager is a good idea...
    std::shared_ptr<FoFiConfigManagerT> mFoFiConfigManager;

    std::shared_ptr<CameraInterfaceT> mCameraDevice;


    RectT<unsigned int> mSelectedRoi; // TODO: Question is if this should be stored here or directly or only inside the camera device...

    boost::signals2::connection mExposureCycleFinishedConnection;

    // TODO: Needs mutex guard!!!-> or atomic?
    FrameT mLastFrame;

    std::vector<std::shared_ptr<MapperFunctionT> > mMapperFunctions;

    std::shared_ptr<ImageConverter16To8T> mImageConverter16To8;

    std::shared_ptr<TaskExecutorT<FocusFinderT> > mFocusFinderExecutor;

    std::optional<PointT<float> > mLastFocusStarPos;

    std::shared_ptr<FocusCurveRecorderLogicT> mFocusCurveRecorderLogic;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_FOCUS_FINDER_LOGIC_H_ */
