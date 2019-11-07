#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_H_

#include <list>
#include <memory>
#include <functional>

#include "exception.h"

#include "camera.h"
#include "focus.h"
#include "filter.h"

#include "focus_finder_profile.h"
#include "focus_curve_record_set.h"
#include "focus_measure_type.h"

DEF_Exception(FocusCurveRecorder);
DEF_Exception(FocusCurveRecorderFailed);
DEF_Exception(FocusCurveRecorderCancelled);

class FocusCurveRecordT;
class FocusCurveRecordSetT;

class FocusCurveRecorderT {
private:
  // Prevent copy of FocusCurveRecorder
  FocusCurveRecorderT(const FocusCurveRecorderT &);
  FocusCurveRecorderT & operator=(const FocusCurveRecorderT &);

  
  typedef boost::signals2::signal<void()> FocusCurveRecorderStartedListenersT;
  FocusCurveRecorderStartedListenersT mFocusCurveRecorderStartedListeners;

  typedef boost::signals2::signal<void(std::shared_ptr<FocusCurveRecordSetT>)> FocusCurveRecorderRecordSetFinishedListenersT;
  FocusCurveRecorderRecordSetFinishedListenersT mFocusCurveRecorderRecordSetFinishedListeners;

  typedef boost::signals2::signal<void(std::shared_ptr<const FocusCurveRecordSetContainerT>)> FocusCurveRecorderFinishedListenersT;
  FocusCurveRecorderFinishedListenersT mFocusCurveRecorderFinishedListeners;
  
  typedef boost::signals2::signal<void()> FocusCurveRecorderCancelledListenersT;
  FocusCurveRecorderCancelledListenersT mFocusCurveRecorderCancelledListeners;

  typedef boost::signals2::signal<void(std::shared_ptr<FocusCurveRecordT>)> FocusCurveRecorderNewRecordListenersT;
  FocusCurveRecorderNewRecordListenersT mFocusCurveRecorderNewRecordListeners;

  typedef boost::signals2::signal<void(std::shared_ptr<FocusCurveRecordSetT>)> FocusCurveRecorderRecordSetUpdateListenersT;
  FocusCurveRecorderRecordSetUpdateListenersT mFocusCurveRecorderRecordSetUpdateListeners;

  
  typedef boost::signals2::signal<void(float, std::string, std::shared_ptr<FocusCurveRecordT>)> FocusCurveRecorderProgressUpdateListenersT;
  FocusCurveRecorderProgressUpdateListenersT mFocusCurveRecorderProgressUpdateListeners;
  
  std::shared_ptr<CameraT> mCamera;
  std::shared_ptr<FocusT> mFocus;
  std::shared_ptr<FilterT> mFilter;

  PointT<float> mLastFocusStarPos;

  FocusFinderProfileT mFocusFinderProfile;

public:
  FocusCurveRecorderT() :
    mCamera(nullptr), mFocus(nullptr), mFilter(nullptr) {
  }

  virtual std::string getName() const = 0;
  virtual ~FocusCurveRecorderT() {
  }
  
  virtual bool isRunning() const = 0;
  virtual void run() = 0;
  virtual void cancel() = 0;
  virtual void reset() = 0;
  virtual std::shared_ptr<const FocusCurveRecordSetContainerT> getFocusCurveRecordSets() const = 0;

  
  std::shared_ptr<CameraT> getCamera() const {
    return mCamera;
  }
  
  void setCamera(std::shared_ptr<CameraT> camera) {
    mCamera = camera;
  }

  std::shared_ptr<FocusT> getFocus() const {
    return mFocus;
  }
  void setFocus(std::shared_ptr<FocusT> focus) {
    mFocus = focus;
  }

  std::shared_ptr<FilterT> getFilter() const {
    return mFilter;
  }
  void setFilter(std::shared_ptr<FilterT> filter) {
    mFilter = filter;
  }

  // TODO: Maybe we find a better name?!
  void setLastFocusStarPos(PointT<float> lastFocusStarPos) {
    mLastFocusStarPos = lastFocusStarPos;
  }
  PointT<float> getLastFocusStarPos() const {
    return mLastFocusStarPos;
  }

  void setFocusFinderProfile(const FocusFinderProfileT & profile) {
    mFocusFinderProfile = profile;
  }
  const FocusFinderProfileT & getFocusFinderProfile() const {
    return mFocusFinderProfile;
  }

  boost::signals2::connection registerFocusCurveRecorderStartedListener(
									const FocusCurveRecorderStartedListenersT::slot_type & inCallBack) {
    return mFocusCurveRecorderStartedListeners.connect(inCallBack);
  }
  template<class T> void unregisterFocusCurveRecorderStartedListener(
								     const T & inCallBack) {
    inCallBack.disconnect();
  }


  boost::signals2::connection registerFocusCurveRecorderRecordSetFinishedListener(
									const FocusCurveRecorderRecordSetFinishedListenersT::slot_type & inCallBack) {
    return mFocusCurveRecorderRecordSetFinishedListeners.connect(inCallBack);
  }
  template<class T> void unregisterFocusCurveRecorderRecordSetFinishedListener(
								     const T & inCallBack) {
    inCallBack.disconnect();
  }


  
    boost::signals2::connection registerFocusCurveRecorderFinishedListener(
									const FocusCurveRecorderFinishedListenersT::slot_type & inCallBack) {
    return mFocusCurveRecorderFinishedListeners.connect(inCallBack);
  }
  template<class T> void unregisterFocusCurveRecorderFinishedListener(
								     const T & inCallBack) {
    inCallBack.disconnect();
  }  

      boost::signals2::connection registerFocusCurveRecorderCancelledListener(
									const FocusCurveRecorderCancelledListenersT::slot_type & inCallBack) {
    return mFocusCurveRecorderCancelledListeners.connect(inCallBack);
  }
  template<class T> void unregisterFocusCurveRecorderCancelledListener(
								     const T & inCallBack) {
    inCallBack.disconnect();
  }

  
  boost::signals2::connection registerFocusCurveRecorderNewRecordListener(
									       const FocusCurveRecorderNewRecordListenersT::slot_type & inCallBack) {
    return mFocusCurveRecorderNewRecordListeners.connect(inCallBack);
  }
  template<class T> void unregisterFocusCurveRecorderNewRecordListener(
									    const T & inCallBack) {
    inCallBack.disconnect();
  }

  boost::signals2::connection registerFocusCurveRecorderRecordSetUpdateListener(
									       const FocusCurveRecorderRecordSetUpdateListenersT::slot_type & inCallBack) {
    return mFocusCurveRecorderRecordSetUpdateListeners.connect(inCallBack);
  }
  template<class T> void unregisterFocusCurveRecorderRecordSetUpdateListener(
									    const T & inCallBack) {
    inCallBack.disconnect();
  }



  boost::signals2::connection registerFocusCurveRecorderProgressUpdateListener(
									       const FocusCurveRecorderProgressUpdateListenersT::slot_type & inCallBack) {
    return mFocusCurveRecorderProgressUpdateListeners.connect(inCallBack);
  }
  template<class T> void unregisterFocusCurveRecorderProgressUpdateListener(
									    const T & inCallBack) {
    inCallBack.disconnect();
  }

  
protected:
  void notifyFocusCurveRecorderStarted() const {
    mFocusCurveRecorderStartedListeners();
  }
  void notifyFocusCurveRecorderRecordSetFinished(std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet) const {
    LOG(debug) << "FocusCurveRecorderT::notifyFocusCurveRecorderRecordSetFinished..." << std::endl;
    
    mFocusCurveRecorderRecordSetFinishedListeners(focusCurveRecordSet);
  }
  void notifyFocusCurveRecorderFinished(std::shared_ptr<const FocusCurveRecordSetContainerT> focusCurveRecordSetContainer) const {
    mFocusCurveRecorderFinishedListeners(focusCurveRecordSetContainer);
  }
  void notifyFocusCurveRecorderCancelled() const {
    mFocusCurveRecorderCancelledListeners();
  }

  void notifyFocusCurveRecorderNewRecord(std::shared_ptr<FocusCurveRecordT> focusCurveRecord) {
    mFocusCurveRecorderNewRecordListeners(focusCurveRecord);
  }

  void notifyFocusCurveRecorderRecordSetUpdate(std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet) {
    LOG(debug) << "FocusCurveRecorderT::notifyFocusCurveRecorderRecordSetUpdate..." << std::endl;
    mFocusCurveRecorderRecordSetUpdateListeners(focusCurveRecordSet);
  }

  void notifyFocusCurveRecorderProgressUpdate(float progress,
				       const std::string & msg, std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
    mFocusCurveRecorderProgressUpdateListeners(progress, msg, focusCurveRecord);
  }
  void notifyFocusCurveRecorderProgressUpdate(const std::string & msg, std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
    mFocusCurveRecorderProgressUpdateListeners(-1.0F, msg, focusCurveRecord);
  }
  void notifyFocusCurveRecorderProgressUpdate(std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
    mFocusCurveRecorderProgressUpdateListeners(-1.0F, "", focusCurveRecord);
  }

  // TODO: Add update...
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_H_*/
