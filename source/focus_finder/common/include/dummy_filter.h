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

#ifndef SOURCE_FOCUS_FINDER_COMMON_DUMMY_FILTER_H_
#define SOURCE_FOCUS_FINDER_COMMON_DUMMY_FILTER_H_

#include <thread>
#include <atomic>
#include <memory>

#include <functional>
#include <utility>
#include <list>

#include "filter.h"
#include "logging.h"
#include "device_connector.h"
#include "dummy_device_connector.h"


class DummyFilterT : virtual public FilterT {

public:
	DummyFilterT();
	virtual ~DummyFilterT();

	std::string getName() const;
	std::shared_ptr<DeviceConnectorT> getConnector() const;


	// Impl. of focus interface
	int getCurrentPos() const;
	int getTargetPos() const;
	void setTargetPos(int inPos);
	bool isMoving() const;

	int getMinPos() const;
	int getMaxPos() const;
	unsigned int getNumSlots() const;

	bool isAbortSupported() const;
	void abortMotion();

private:
	void filterMovementLoop();

	std::thread filterMovementThread;
	bool mCancelFilterMovementLoop; 	// TODO: ATOMIC

	int mTargetPos;
	int mCurrentPos;
	bool mMovementAborted; // TODO: ATOMIC?!
	bool mMovementRunning; // TODO: ATOMIC?!

	std::shared_ptr<DummyDeviceConnectorT> mDummyConnector;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_DUMMY_FILTER_H_ */
