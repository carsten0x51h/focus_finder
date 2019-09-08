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

#include "camera.h"
#include "focus.h"
#include "filter.h"
#include "focus_finder_profile.h"

class FocusFinderRecordT;

// TODO: Question is, if FocusFinder should have a callback for "exposure finished"... actually it should be implemented in sub class....
//       But problem is, that registering/unregistering the callback needs to be repeated when mCamera changes - setCamera()
//       -> overwrite setCamera and call super method?

class FocusFinderT {
private:
	// Prevent copy of FocusFinder
	FocusFinderT(const FocusFinderT &);
	FocusFinderT & operator=(const FocusFinderT &);

	typedef boost::signals2::signal<void()> FocusFinderStartedListenersT;
	FocusFinderStartedListenersT mFocusFinderStartedListeners;

	typedef boost::signals2::signal<void(float, std::string, std::shared_ptr<FocusFinderRecordT>)> FocusFinderProgressUpdateListenersT;
	FocusFinderProgressUpdateListenersT mFocusFinderProgressUpdateListeners;

	typedef boost::signals2::signal<void()> FocusFinderFinishedListenersT;
	FocusFinderFinishedListenersT mFocusFinderFinishedListeners;

	typedef boost::signals2::signal<void()> FocusFinderCancelledListenersT;
	FocusFinderCancelledListenersT mFocusFinderCancelledListeners;

	std::shared_ptr<CameraT> mCamera;
	std::shared_ptr<FocusT> mFocus;
	std::shared_ptr<FilterT> mFilter;

	FocusFinderProfileT mFocusFinderProfile;

	PointT<float> mLastFocusStarPos;

public:
	FocusFinderT() :
			mCamera(nullptr), mFocus(nullptr), mFilter(nullptr) {
	}

	virtual std::string getName() const = 0;
	virtual ~FocusFinderT() {
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
			const std::string & msg, std::shared_ptr<FocusFinderRecordT> focusFinderRecord = nullptr) const {
		mFocusFinderProgressUpdateListeners(progress, msg, focusFinderRecord);
	}
	void notifyFocusFinderFinished() const {
		mFocusFinderFinishedListeners();
	}
	void notifyFocusFinderCancelled() const {
		mFocusFinderCancelledListeners();
	}
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_H_ */
