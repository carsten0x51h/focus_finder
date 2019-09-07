/*
 * filter.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FILTER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FILTER_H_

#include <boost/signals2.hpp>

#include "exception.h"
#include "device.h"
#include "enum_helper.h"

class FilterT : public DeviceT {
private:
	typedef boost::signals2::signal<void (int /*new pos*/)> FilterPositionChangedListenersT;
	typedef boost::signals2::signal<void (int /*target pos*/)> TargetPositionReachedListenersT;
	typedef boost::signals2::signal<void (int /*current pos?*/)> FilterMovementAbortedListenersT;

	FilterPositionChangedListenersT mFilterPositionChangedListeners;
	TargetPositionReachedListenersT mTargetPositionReachedListeners;
	FilterMovementAbortedListenersT mFilterMovementAbortedListeners;

	// We do not want device copies
	FilterT(const FilterT &);
	FilterT & operator=(const FilterT &);

public:
	FilterT() {}

	/**
	 * Filter position
	 */
	virtual int getCurrentPos() const = 0;
	virtual int getTargetPos() const = 0;

	// NOTE: Calling this set-function twice causes 2 movements of Atik wheel... There is also no "abort" function
	//       implemented in INDI for the ATIK Wheel.
	virtual void setTargetPos(int inPos) = 0;

	virtual bool isMoving() const = 0;

	/**
	 * Filter properties
	 */
	virtual int getMinPos() const = 0;
	virtual int getMaxPos() const = 0;
	virtual unsigned int getNumSlots() const = 0;

	/**
	* Abort handling
	*/
	virtual bool isAbortSupported() const = 0;
	virtual void abortMotion() = 0;


	/**
	 * Filter movement events
	 */
	boost::signals2::connection registerFilterPositionChangedListener(const FilterPositionChangedListenersT::slot_type & inCallBack) {
		return mFilterPositionChangedListeners.connect(inCallBack);
	}
	template <class T> void unregisterFilterPositionChangedListener(const T & inCallBack) {
		//mFilterPositionChangedListeners.disconnect(inCallBack);
		inCallBack.disconnect();
	}

	boost::signals2::connection registerTargetPositionReachedListener(const TargetPositionReachedListenersT::slot_type & inCallBack) {
		return mTargetPositionReachedListeners.connect(inCallBack);
	}
	template <class T> void unregisterTargetPositionReachedListener(const T & inCallBack) {
		//mTargetPositionReachedListeners.disconnect(inCallBack);
		inCallBack.disconnect();
	}

	boost::signals2::connection registerFilterMovementAbortedListener(const FilterMovementAbortedListenersT::slot_type & inCallBack) {
		return mFilterMovementAbortedListeners.connect(inCallBack);
	}
	template <class T> void unregisterFilterMovementAbortedListener(const T & inCallBack) {
		//mFilterMovementAbortedListeners.disconnect(inCallBack);
		inCallBack.disconnect();
	}

protected:
	void notifyFilterPositionChanged(int currentPosition) const { mFilterPositionChangedListeners(currentPosition); }
	void notifyTargetPositionReached(int targetPosition) const { mTargetPositionReachedListeners(targetPosition); }
	void notifyFilterMovementAborted(int currentPosition) const { mFilterMovementAbortedListeners(currentPosition); }
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FILTER_H_ */
