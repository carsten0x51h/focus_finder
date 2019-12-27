#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_ANALYZER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_ANALYZER_H_

#include <list>
#include <memory>
#include <functional>
#include <boost/signals2.hpp>
#include <condition_variable>

#include "exception.h"

#include "camera.h"
#include "focus.h"
#include "filter.h"

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
  FocusControllerT & operator=(const FocusControllerT &);

  std::shared_ptr<CameraT> mCamera;
  std::shared_ptr<FocusT> mFocus;
  std::shared_ptr<FilterT> mFilter;

  PointFT mLastFocusStarPos;
  FocusFinderProfileT mFocusFinderProfile;


  // TODO: Rename FocusCurveRecordT to FocusControllerRecordT? Or just RecordT? or StarRecordT?
  typedef boost::signals2::signal<void(float, std::string, std::shared_ptr<FocusCurveRecordT>)> FocusControllerProgressUpdateListenersT;
  FocusControllerProgressUpdateListenersT mFocusControllerProgressUpdateListeners;
  
  typedef boost::signals2::signal<void(std::shared_ptr<FocusCurveRecordT>)> FocusControllerNewRecordListenersT;
  FocusControllerNewRecordListenersT mFocusControllerNewRecordListeners;

    
  void waitForFocus(std::chrono::milliseconds timeout) const;
  void onImageReceived(RectT<unsigned int> roi, std::shared_ptr<const ImageT> image, bool lastFrame);
  BoundaryLocationT::TypeE determineBoundaryLoc(float lowerFocusMeasure, float upperFocusMeasure, float focusMeasure) const;

  
  std::shared_ptr<const ImageT> mCurrentImage;
  
  boost::signals2::connection mCameraExposureFinishedConnection;
  
  std::atomic<bool> mCancelled;
  std::condition_variable cv;
  std::mutex cvMutex;
  
  int mInitialFocusPos;

 protected:
  void notifyFocusControllerProgressUpdate(float progress,
				       const std::string & msg, std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
    mFocusControllerProgressUpdateListeners(progress, msg, focusCurveRecord);
  }
  void notifyFocusControllerProgressUpdate(const std::string & msg, std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
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
  FocusControllerT(std::shared_ptr<CameraT> camera, std::shared_ptr<FocusT> focus, std::shared_ptr<FilterT> filter);
  ~FocusControllerT();

  void cancel();
  
  std::shared_ptr<CameraT> getCamera() const;
  //  void setCamera(std::shared_ptr<CameraT> camera);

  std::shared_ptr<FocusT> getFocus() const;
  //  void setFocus(std::shared_ptr<FocusT> focus);

  std::shared_ptr<FilterT> getFilter() const;
  //  void setFilter(std::shared_ptr<FilterT> filter);
  
  // TODO: Maybe we find a better name?!
  void setLastFocusStarPos(PointFT lastFocusStarPos);
  PointFT getLastFocusStarPos() const;

  void setFocusFinderProfile(const FocusFinderProfileT & profile);
  const FocusFinderProfileT & getFocusFinderProfile() const;


  void devicesAvailabilityCheck();
  
  void checkIfStarIsThere(const ImageT & img, float * outSnr = 0) const;
  void runExposureBlocking(std::chrono::milliseconds expTime);
  void moveFocusByBlocking(FocusDirectionT::TypeE direction, int ticks, std::chrono::milliseconds timeout);
  void moveFocusToBlocking(int absPos, std::chrono::milliseconds timeout);
  
  void checkCancelled() const;
  
  SelfOrientationResultT performSelfOrientation();

  // TODO: Could there also be a "binary search" to be faster than "boundaryScanLinear" even without having a curve?!
  void boundaryScanLinear(const SelfOrientationResultT & selfOrientationResult, float stepSize, float focusMeasureLimit);

  int boundaryScanWithFocusCurveSupport(std::shared_ptr<CurveFunctionT> focusCurveFunction, const SelfOrientationResultT & selfOrientationResult, FocusMeasureTypeT::TypeE curveFocusMeasureType, float focusMeasureLimit, float focusMeasureDelta = 1.5F);
  

  std::shared_ptr<FocusCurveRecordT> measureFocus();
  
  void cleanup(); // NOTE: Called from destructor


  
  boost::signals2::connection registerFocusControllerProgressUpdateListener(
									       const FocusControllerProgressUpdateListenersT::slot_type & inCallBack) {
    return mFocusControllerProgressUpdateListeners.connect(inCallBack);
  }
  template<class T> void unregistermFocusControllerProgressUpdateListener(
									    const T & inCallBack) {
    inCallBack.disconnect();
  }

  boost::signals2::connection registerFocusControllerNewRecordListener(
									       const FocusControllerNewRecordListenersT::slot_type & inCallBack) {
    return mFocusControllerNewRecordListeners.connect(inCallBack);
  }
  template<class T> void unregisterFocusControllerNewRecordListener(
									    const T & inCallBack) {
    inCallBack.disconnect();
  }
};



#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_ANALYZER_H_*/