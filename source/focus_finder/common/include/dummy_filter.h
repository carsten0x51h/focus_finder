/*
 * indi_camera.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

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
