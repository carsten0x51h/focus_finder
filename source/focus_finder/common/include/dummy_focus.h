/*
 * indi_camera.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_DUMMY_FOCUS_H_
#define SOURCE_FOCUS_FINDER_COMMON_DUMMY_FOCUS_H_

#include <thread>
#include <atomic>
#include <memory>

#include <functional>
#include <utility>
#include <list>

#include "focus.h"
#include "logging.h"
#include "device_connector.h"
#include "dummy_device_connector.h"


class DummyFocusT : virtual public FocusT {

public:
	DummyFocusT();
	virtual ~DummyFocusT();

	std::string getName() const;
	std::shared_ptr<DeviceConnectorT> getConnector() const;


	// Impl. of focus interface
	bool isTemperatureSupported() const;
	float getTemperature() const;

	bool isMoving() const;

	bool isAbsPosSupported() const;
	int getCurrentPos() const;
	int getTargetPos() const;
	void setTargetPos(unsigned int inTicks, FocusDirectionT::TypeE direction);
	void setTargetPos(int inAbsPos);
	void resetPositionCounter();
	int getMinAbsPos() const;
	int getMaxAbsPos() const;

	bool isAbortSupported() const;
	void abortMotion();

private:
	void focusLoop();

	std::thread focusingThread;
	bool mCancelFocusLoop; 	// TODO: ATOMIC

	int mTargetPos;
	int mCurrentPos;
	bool mMovementAborted; // TODO: ATOMIC?!
	bool mMovementRunning; // TODO: ATOMIC?!

	std::shared_ptr<DummyDeviceConnectorT> mDummyConnector;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_DUMMY_FOCUS_H_ */
