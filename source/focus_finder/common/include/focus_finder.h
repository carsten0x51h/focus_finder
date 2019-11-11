/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_H_

#include <memory>
#include <functional>

#include "exception.h"

class FocusCurveRecordT;
class FocusAnalyzerT;

//DEF_Exception(FocusFinderCancelled);

class FocusFinderT {
private:
  // Prevent copy of FocusFinder
  FocusFinderT(const FocusFinderT &);
  FocusFinderT & operator=(const FocusFinderT &);

  typedef boost::signals2::signal<void()> FocusFinderStartedListenersT;
  FocusFinderStartedListenersT mFocusFinderStartedListeners;

  typedef boost::signals2::signal<void(float, std::string, std::shared_ptr<FocusCurveRecordT>)> FocusFinderProgressUpdateListenersT;
  FocusFinderProgressUpdateListenersT mFocusFinderProgressUpdateListeners;

  typedef boost::signals2::signal<void()> FocusFinderFinishedListenersT;
  FocusFinderFinishedListenersT mFocusFinderFinishedListeners;

  typedef boost::signals2::signal<void()> FocusFinderCancelledListenersT;
  FocusFinderCancelledListenersT mFocusFinderCancelledListeners;

  std::shared_ptr<FocusAnalyzerT> mFocusAnalyzer;

public:
  FocusFinderT(std::shared_ptr<FocusAnalyzerT> focusAnalyzer) : mFocusAnalyzer(focusAnalyzer) {
  }

  virtual ~FocusFinderT() {
  }
  std::shared_ptr<FocusAnalyzerT> getFocusAnalyzer() { return mFocusAnalyzer; }
  
  virtual std::string getName() const = 0;
  virtual bool isRunning() const = 0;
  virtual void run() = 0;
  virtual void cancel() = 0;
  virtual void reset() = 0;


  boost::signals2::connection registerFocusFinderStartedListener(
								 const FocusFinderStartedListenersT::slot_type & inCallBack) {
    return mFocusFinderStartedListeners.connect(inCallBack);
  }
  template<class T> void unregisterFocusFinderStartedListener(
							      const T & inCallBack) {
    inCallBack.disconnect();
  }

  boost::signals2::connection registerFocusFinderProgressUpdateListener(
									const FocusFinderProgressUpdateListenersT::slot_type & inCallBack) {
    return mFocusFinderProgressUpdateListeners.connect(inCallBack);
  }
  template<class T> void unregisterFocusFinderProgressUpdateListener(
								     const T & inCallBack) {
    inCallBack.disconnect();
  }

  boost::signals2::connection registerFocusFinderFinishedListener(
								  const FocusFinderFinishedListenersT::slot_type & inCallBack) {
    return mFocusFinderFinishedListeners.connect(inCallBack);
  }
  template<class T> void unregisterFocusFinderFinishedListener(
							       const T & inCallBack) {
    inCallBack.disconnect();
  }

  boost::signals2::connection registerFocusFinderCancelledListener(
								   const FocusFinderCancelledListenersT::slot_type & inCallBack) {
    return mFocusFinderCancelledListeners.connect(inCallBack);
  }
  template<class T> void unregisterFocusFinderCancelledListener(
								const T & inCallBack) {
    inCallBack.disconnect();
  }

  // TODO: Add Failed...

protected:
  void notifyFocusFinderStarted() const {
    mFocusFinderStartedListeners();
  }
  void notifyFocusFinderProgressUpdate(float progress,
				       const std::string & msg, std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
    mFocusFinderProgressUpdateListeners(progress, msg, focusCurveRecord);
  }
  void notifyFocusFinderFinished() const {
    mFocusFinderFinishedListeners();
  }
  void notifyFocusFinderCancelled() const {
    mFocusFinderCancelledListeners();
  }
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_H_ */
