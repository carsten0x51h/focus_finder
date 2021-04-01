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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_ANALYZER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_ANALYZER_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_ANALYZER_H_

#include <atomic>
#include <list>
#include <memory>
#include <functional>
#include <boost/signals2.hpp>
#include <condition_variable>

#include "exception.h"

#include "camera_interface.h"
#include "focus_interface.h"
#include "filter_interface.h"

#include "focus_measure_type.h"
#include "focus_finder_profile.h"
#include "self_orientation_result.h"
#include "boundary_location.h"

class CurveFunctionT;

// TODO: Question is, if FocusFinder should have a callback for "exposure finished"... actually it should be implemented in sub class....
//       But problem is, that registering/unregistering the callback needs to be repeated when mCamera changes - setCamera()
//       -> overwrite setCamera and call super method?

DEF_Exception(FocusController);

DEF_Exception(FocusControllerFailed);

DEF_Exception(FocusControllerCancelled);

class FocusControllerT {
private:
    // Prevent copy of FocusControllerT
    FocusControllerT(const FocusControllerT &);

    FocusControllerT &operator=(const FocusControllerT &);

    std::shared_ptr<CameraInterfaceT> mCamera;
    std::shared_ptr<FocusInterfaceT> mFocus;
    std::shared_ptr<FilterInterfaceT> mFilter;

    PointFT mLastFocusStarPos;
    FocusFinderProfileT mFocusFinderProfile;


    // TODO: Rename FocusCurveRecordT to FocusControllerRecordT? Or just RecordT? or StarRecordT?
    typedef boost::signals2::signal<void(float, std::string,
                                         std::shared_ptr<FocusCurveRecordT>)> FocusControllerProgressUpdateListenersT;
    FocusControllerProgressUpdateListenersT mFocusControllerProgressUpdateListeners;

    typedef boost::signals2::signal<void(std::shared_ptr<FocusCurveRecordT>)> FocusControllerNewRecordListenersT;
    FocusControllerNewRecordListenersT mFocusControllerNewRecordListeners;


    void waitForFocus(std::chrono::milliseconds timeout, bool ignoreCancel = false) const;

    void onImageReceived(RectT<unsigned int> roi, std::shared_ptr<const ImageT> image, bool lastFrame);

    static BoundaryLocationT::TypeE
    determineBoundaryLoc(float lowerFocusMeasure, float upperFocusMeasure, float focusMeasure) ;


    std::shared_ptr<const ImageT> mCurrentImage;

    boost::signals2::connection mCameraExposureFinishedConnection;

    std::atomic<bool> mCancelled;
    std::condition_variable cv;
    std::mutex cvMutex;

    int mInitialFocusPos;

protected:
    void notifyFocusControllerProgressUpdate(float progress,
                                             const std::string &msg,
                                             std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
        mFocusControllerProgressUpdateListeners(progress, msg, focusCurveRecord);
    }

    void notifyFocusControllerProgressUpdate(const std::string &msg,
                                             std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
        mFocusControllerProgressUpdateListeners(-1.0F, msg, focusCurveRecord);
    }

    void notifyFocusControllerProgressUpdate(std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
        mFocusControllerProgressUpdateListeners(-1.0F, "", focusCurveRecord);
    }

    void notifyFocusControllerNewRecord(std::shared_ptr<FocusCurveRecordT> focusCurveRecord) {
        mFocusControllerNewRecordListeners(focusCurveRecord);
    }

public:
    //FocusControllerT();
    FocusControllerT(std::shared_ptr<CameraInterfaceT> camera, std::shared_ptr<FocusInterfaceT> focus,
                     std::shared_ptr<FilterInterfaceT> filter);

    ~FocusControllerT();

    void cancel();

    std::shared_ptr<CameraInterfaceT> getCamera() const;
    //  void setCamera(std::shared_ptr<CameraInterfaceT> camera);

    std::shared_ptr<FocusInterfaceT> getFocus() const;
    //  void setFocus(std::shared_ptr<FocusInterfaceT> focus);

    std::shared_ptr<FilterInterfaceT> getFilter() const;
    //  void setFilter(std::shared_ptr<FilterInterfaceT> filter);

    // TODO: Maybe we find a better name?!
    void setLastFocusStarPos(PointFT lastFocusStarPos);

    PointFT getLastFocusStarPos() const;

    void setFocusFinderProfile(const FocusFinderProfileT &profile);

    const FocusFinderProfileT &getFocusFinderProfile() const;


    void devicesAvailabilityCheck() const;

    void checkIfStarIsThere(const ImageT &img, float *outSnr = 0) const;

    void runExposureBlocking(std::chrono::milliseconds expTime);

    void moveFocusByBlocking(FocusDirectionT::TypeE direction, int ticks, std::chrono::milliseconds timeout);

    void moveFocusToBlocking(int absPos, std::chrono::milliseconds timeout, bool ignoreCancel = false);

    void checkCancelled() const;

    SelfOrientationResultT performSelfOrientation(float focusMeasureLimit);

    // TODO: Could there also be a "binary search" to be faster than "boundaryScanLinear" even without having a curve?!
    void
    boundaryScanLinear(const SelfOrientationResultT &selfOrientationResult, float stepSize, float focusMeasureLimit);

    int boundaryScanWithFocusCurveSupport(const std::shared_ptr<CurveFunctionT>& focusCurveFunction,
                                          const SelfOrientationResultT &selfOrientationResult,
                                          FocusMeasureTypeT::TypeE curveFocusMeasureType, float focusMeasureLimit,
                                          float focusMeasureDelta = 1.5F);


    std::shared_ptr<FocusCurveRecordT> measureFocus();

    void cleanup(); // NOTE: Called from destructor



    boost::signals2::connection registerFocusControllerProgressUpdateListener(
            const FocusControllerProgressUpdateListenersT::slot_type &inCallBack) {
        return mFocusControllerProgressUpdateListeners.connect(inCallBack);
    }

    template<class T>
    void unregistermFocusControllerProgressUpdateListener(
            const T &inCallBack) {
        inCallBack.disconnect();
    }

    boost::signals2::connection registerFocusControllerNewRecordListener(
            const FocusControllerNewRecordListenersT::slot_type &inCallBack) {
        return mFocusControllerNewRecordListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterFocusControllerNewRecordListener(
            const T &inCallBack) {
        inCallBack.disconnect();
    }
};


#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_ANALYZER_H_*/
