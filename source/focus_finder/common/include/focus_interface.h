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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_INTERFACE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_INTERFACE_H_

#include <boost/signals2.hpp>

#include "exception.h"
#include "device.h"
#include "focus_direction.h"

DEF_Exception(Focus);


class FocusInterfaceT : public DeviceInterfaceT {
private:
	typedef boost::signals2::signal<void (int /*target pos*/)> TargetPositionReachedListenersT;
	typedef boost::signals2::signal<void (int /*new pos*/)> FocusPositionChangedListenersT;
	typedef boost::signals2::signal<void (int /*current pos?*/)> FocusMovementAbortedListenersT;

	TargetPositionReachedListenersT mTargetPositionReachedListeners;
	FocusPositionChangedListenersT mFocusPositionChangedListeners;
	FocusMovementAbortedListenersT mFocusMovementAbortedListeners;

	// We do not want device copies
	FocusInterfaceT(const FocusInterfaceT &);
	FocusInterfaceT & operator=(const FocusInterfaceT &);

public:
	FocusInterfaceT() {}

	/**
	 * Temperature
	 */
	virtual bool isTemperatureSupported() const = 0;
	virtual float getTemperature() const = 0;

	/**
	* Generic
	*/
	virtual bool isMoving() const = 0;

	/**
	* Positioning
	*/
	virtual bool isAbsPosSupported() const = 0;
	virtual int getCurrentPos() const = 0;
	virtual int getTargetPos() const = 0;
	virtual void setTargetPos(unsigned int inTicks, FocusDirectionT::TypeE direction) = 0;
	virtual void setTargetPos(int inAbsPos) = 0;
	virtual void resetPositionCounter() = 0;
	virtual int getMinAbsPos() const = 0;
	virtual int getMaxAbsPos() const = 0;


	/**
	* Movement event handling
	*/
	// FocusMoved event
	boost::signals2::connection registerFocusPositionChangedListener(const FocusPositionChangedListenersT::slot_type & inCallBack) {
		return mFocusPositionChangedListeners.connect(inCallBack);
	}
	template <class T> void unregisterFocusPositionChangedListener(const T & inCallBack) {
		//mFocusPositionChangedListeners.disconnect(inCallBack);
		inCallBack.disconnect();
	}

	// TargetPosReached event
	boost::signals2::connection registerTargetPositionReachedListener(const TargetPositionReachedListenersT::slot_type & inCallBack) {
		return mTargetPositionReachedListeners.connect(inCallBack);
	}
	template <class T> void unregisterTargetPositionReachedListener(const T & inCallBack) {
		//mTargetPositionReachedListeners.disconnect(inCallBack);
		inCallBack.disconnect();
	}


	/**
	* Abort handling
	*/
	virtual bool isAbortSupported() const = 0;
	virtual void abortMotion() = 0;

	// Motion cancelled event
	boost::signals2::connection registerFocusMovementAbortedListener(const FocusMovementAbortedListenersT::slot_type & inCallBack) {
		return mFocusMovementAbortedListeners.connect(inCallBack);
	}
	template <class T> void unregisterFocusMovementAbortedListener(const T & inCallBack) {
		//mFocusMovementAbortedListeners.disconnect(inCallBack);
		inCallBack.disconnect();
	}

  // Idea to simplify the unregistering of each single listener to all the events in case the device is switched.
  void clearListeners() {
    mTargetPositionReachedListeners.disconnect_all_slots();
    mFocusPositionChangedListeners.disconnect_all_slots();
    mFocusMovementAbortedListeners.disconnect_all_slots();
  }

protected:
	void notifyTargetPositionReached(int currentPosition) const { mTargetPositionReachedListeners(currentPosition); }
	void notifyFocusPositionChanged(int currentPosition) const { mFocusPositionChangedListeners(currentPosition); }
	void notifyFocusMovementAborted(int currentPosition) const { mFocusMovementAbortedListeners(currentPosition); }
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_INTERFACE_H_ */
