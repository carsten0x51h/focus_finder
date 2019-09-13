#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_H_

#include <memory>
#include <functional>

#include "exception.h"

#include "camera.h"
#include "focus.h"
#include "filter.h"

DEF_Exception(FocusCurveRecorderCancelled);

class FocusCurveRecorderT {
private:
  // Prevent copy of FocusFinder
  FocusCurveRecorderT(const FocusCurveRecorderT &);
  FocusCurveRecorderT & operator=(const FocusCurveRecorderT &);

  
  typedef boost::signals2::signal<void()> FocusCurveRecorderStartedListenersT;
  FocusCurveRecorderStartedListenersT mFocusCurveRecorderStartedListeners;

  typedef boost::signals2::signal<void(bool)> FocusCurveRecorderFinishedListenersT;
  FocusCurveRecorderFinishedListenersT mFocusCurveRecorderFinishedListeners;

  typedef boost::signals2::signal<void()> FocusCurveRecorderCancelledListenersT;
  FocusCurveRecorderCancelledListenersT mFocusCurveRecorderCancelledListeners;

  std::shared_ptr<CameraT> mCamera;
  std::shared_ptr<FocusT> mFocus;
  std::shared_ptr<FilterT> mFilter;

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



  boost::signals2::connection registerFocusCurveRecorderStartedListener(
									const FocusCurveRecorderStartedListenersT::slot_type & inCallBack) {
    return mFocusCurveRecorderStartedListeners.connect(inCallBack);
  }
  template<class T> void unregisterFocusCurveRecorderStartedListener(
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

protected:
  void notifyFocusCurveRecorderStarted() const {
    mFocusCurveRecorderStartedListeners();
  }
  void notifyFocusCurveRecorderFinished(bool lastCurve) const {
    mFocusCurveRecorderFinishedListeners(lastCurve);
  }
  void notifyFocusCurveRecorderCancelled() const {
    mFocusCurveRecorderCancelledListeners();
  }

  // TODO: Add update...
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_H_*/
