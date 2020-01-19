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
#define SOURCE_FOCUS_FINDER_COMMON_FOCUS_FINDER_LOGIC_H_

#include <memory>

#include <boost/signals2.hpp>

#include "star_info.h"
#include "image.h"
#include "rect.h"
#include "frame.h"
#include "size.h"
#include "task_executor.h"

class CameraT;
class FocusT;
class FilterT;
class DeviceManagerT;
class ProfileManagerT;
class MapperFunctionT;
class ImageConverter16To8T;
class FocusFinderT;
class FocusCurveRecorderLogicT;

class FocusFinderLogicT
{
public:
  FocusFinderLogicT();
  ~FocusFinderLogicT();

  std::shared_ptr<CameraT> getCurrentCamera();
  std::shared_ptr<FocusT> getCurrentFocus();
  std::shared_ptr<FilterT> getCurrentFilter();

  std::shared_ptr<DeviceManagerT> getDeviceManager() const;
  std::shared_ptr<ProfileManagerT> getProfileManager() const;

  void setSelectedRoi(const RectT<unsigned int> & roi);
  RectT<unsigned int> getSelectedRoi() const;
  void clearSelectedRoi();

  std::optional<PointT<float> > findFocusStar(const PointT<float> & poi);

  const FrameT & getLastFrame() const;


  std::vector<std::string> getMapperFunctionNames() const;

  std::shared_ptr<MapperFunctionT>
  getMapperFunctionByName(const std::string & mapperFunctionName) const;

  std::shared_ptr<ImageConverter16To8T> getImageConverter16To8() const;

  std::shared_ptr<TaskExecutorT<FocusFinderT> > getFocusFinderExecutor() const;

  std::optional<PointT<float> > getLastFocusStarPos() const;

  std::shared_ptr<FocusCurveRecorderLogicT> getFocusCurveRecorderLogic();


  static void init();
  static void close();

  /**
   * Example usage:
   *
   * FocusFinderLogicT::get()->getProfileManager()...
   */
  static FocusFinderLogicT * get();
  
private:
  static FocusFinderLogicT * sSelf;

  size_t calcNumStarsInRegion(const ImageT & inImg) const;
  void initImageMappers();
  void updateProfile();

  // TODO... remove?
  std::shared_ptr<DeviceManagerT> mDeviceManager;

  // TODO: Not yet sure, if sep. of DeviceManager and ProfileManager is a good idea...
  std::shared_ptr<ProfileManagerT> mProfileManager;

  std::shared_ptr<CameraT> mCameraDevice;


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
