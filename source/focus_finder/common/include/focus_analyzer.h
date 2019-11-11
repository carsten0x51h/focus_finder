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

// TODO: Question is, if FocusFinder should have a callback for "exposure finished"... actually it should be implemented in sub class....
//       But problem is, that registering/unregistering the callback needs to be repeated when mCamera changes - setCamera()
//       -> overwrite setCamera and call super method?

DEF_Exception(FocusAnalyzer);
DEF_Exception(FocusAnalyzerFailed);
DEF_Exception(FocusAnalyzerCancelled);

class FocusAnalyzerT {
private:
  // Prevent copy of FocusAnalyzerT
  FocusAnalyzerT(const FocusAnalyzerT &);
  FocusAnalyzerT & operator=(const FocusAnalyzerT &);

  std::shared_ptr<CameraT> mCamera;
  std::shared_ptr<FocusT> mFocus;
  std::shared_ptr<FilterT> mFilter;

  PointFT mLastFocusStarPos;
  FocusFinderProfileT mFocusFinderProfile;


  // TODO: Rename FocusCurveRecordT to FocusAnalyzerRecordT? Or just RecordT? or StarRecordT?
  typedef boost::signals2::signal<void(float, std::string, std::shared_ptr<FocusCurveRecordT>)> FocusAnalyzerProgressUpdateListenersT;
  FocusAnalyzerProgressUpdateListenersT mFocusAnalyzerProgressUpdateListeners;
  
  typedef boost::signals2::signal<void(std::shared_ptr<FocusCurveRecordT>)> FocusAnalyzerNewRecordListenersT;
  FocusAnalyzerNewRecordListenersT mFocusAnalyzerNewRecordListeners;

    
  void waitForFocus(std::chrono::milliseconds timeout) const;
  void onImageReceived(RectT<unsigned int> roi, std::shared_ptr<const ImageT> image, bool lastFrame);
  
  std::shared_ptr<const ImageT> mCurrentImage;
  
  boost::signals2::connection mCameraExposureFinishedConnection;
  
  std::atomic<bool> mCancelled;
  std::condition_variable cv;
  std::mutex cvMutex;
  
  int mInitialFocusPos;

 protected:
  void notifyFocusAnalyzerProgressUpdate(float progress,
				       const std::string & msg, std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
    mFocusAnalyzerProgressUpdateListeners(progress, msg, focusCurveRecord);
  }
  void notifyFocusAnalyzerProgressUpdate(const std::string & msg, std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
    mFocusAnalyzerProgressUpdateListeners(-1.0F, msg, focusCurveRecord);
  }
  void notifyFocusAnalyzerProgressUpdate(std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
    mFocusAnalyzerProgressUpdateListeners(-1.0F, "", focusCurveRecord);
  }
  void notifyFocusAnalyzerNewRecord(std::shared_ptr<FocusCurveRecordT> focusCurveRecord) {
    mFocusAnalyzerNewRecordListeners(focusCurveRecord);
  }

public:
  //FocusAnalyzerT();
  FocusAnalyzerT(std::shared_ptr<CameraT> camera, std::shared_ptr<FocusT> focus, std::shared_ptr<FilterT> filter);
  ~FocusAnalyzerT();

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
  void moveUntilFocusMeasureLimitReached(const SelfOrientationResultT & selfOrientationResult, float stepSize, float focusMeasureLimit);
  std::shared_ptr<FocusCurveRecordT> measureFocus();
  
  void cleanup(); // NOTE: Called from destructor


  
  boost::signals2::connection registerFocusAnalyzerProgressUpdateListener(
									       const FocusAnalyzerProgressUpdateListenersT::slot_type & inCallBack) {
    return mFocusAnalyzerProgressUpdateListeners.connect(inCallBack);
  }
  template<class T> void unregistermFocusAnalyzerProgressUpdateListener(
									    const T & inCallBack) {
    inCallBack.disconnect();
  }

  boost::signals2::connection registerFocusAnalyzerNewRecordListener(
									       const FocusAnalyzerNewRecordListenersT::slot_type & inCallBack) {
    return mFocusAnalyzerNewRecordListeners.connect(inCallBack);
  }
  template<class T> void unregisterFocusAnalyzerNewRecordListener(
									    const T & inCallBack) {
    inCallBack.disconnect();
  }
};



#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_ANALYZER_H_*/
