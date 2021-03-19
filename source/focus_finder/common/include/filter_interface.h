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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FILTER_INTERFACE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FILTER_INTERFACE_H_

#include <boost/signals2.hpp>

#include "exception.h"
#include "device.h"
#include "enum_helper.h"

class FilterInterfaceT : public DeviceInterfaceT {
private:
	typedef boost::signals2::signal<void (int /*new pos*/)> FilterPositionChangedListenersT;
	typedef boost::signals2::signal<void (int /*target pos*/)> TargetPositionReachedListenersT;
	typedef boost::signals2::signal<void (int /*current pos?*/)> FilterMovementAbortedListenersT;

	FilterPositionChangedListenersT mFilterPositionChangedListeners;
	TargetPositionReachedListenersT mTargetPositionReachedListeners;
	FilterMovementAbortedListenersT mFilterMovementAbortedListeners;

	// We do not want device copies
	FilterInterfaceT(const FilterInterfaceT &);
	FilterInterfaceT & operator=(const FilterInterfaceT &);

public:
	FilterInterfaceT() {}

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

  // Idea to simplify the unregistering of each single listener to all the events in case the device is switched.
  void clearListeners() {
    mFilterPositionChangedListeners.disconnect_all_slots();
    mTargetPositionReachedListeners.disconnect_all_slots();
    mFilterMovementAbortedListeners.disconnect_all_slots();
  }

protected:
	void notifyFilterPositionChanged(int currentPosition) const { mFilterPositionChangedListeners(currentPosition); }
	void notifyTargetPositionReached(int targetPosition) const { mTargetPositionReachedListeners(targetPosition); }
	void notifyFilterMovementAborted(int currentPosition) const { mFilterMovementAbortedListeners(currentPosition); }
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FILTER_INTERFACE_H_ */
