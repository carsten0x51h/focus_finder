/*
 * dummy_focus.cpp
 *
 *  Created on: Apr 20, 2019
 *      Author: devnull
 */

#include <thread>
#include <atomic>
#include <memory>
#include <chrono>

#include <functional>
#include <utility>
#include <list>

#include "include/dummy_focus.h"


using namespace std::chrono_literals;

DummyFocusT::DummyFocusT() :
	mCancelFocusLoop(false),
	mTargetPos(0),
	mCurrentPos(0),
	mMovementAborted(false),
	mMovementRunning(false) {

	mDummyConnector = std::make_shared<DummyDeviceConnectorT>();

	// Read connection status etc. from driver?

	focusingThread = std::thread(&DummyFocusT::focusLoop, this);
	focusingThread.detach();
}

DummyFocusT::~DummyFocusT() {

	// TODO: ATOMIC
	mCancelFocusLoop = true;
}

/////////////////////////////////////////////////
// Device
/////////////////////////////////////////////////
std::string DummyFocusT::getName() const {
	return "MyDummyFocusName";
}

/**
 * Temperature
 */
bool DummyFocusT::isTemperatureSupported() const {
	return false;
}

float DummyFocusT::getTemperature() const {
	// TODO: Throw if not supported?
	// TODO: Add listener - when temperature has changed...
	return 0;
}

/**
* Generic
*/
bool DummyFocusT::isMoving() const {
	return mMovementRunning;
}

/**
* Positioning
*/
bool DummyFocusT::isAbsPosSupported() const {
	return true;
}

int DummyFocusT::getCurrentPos() const {
	return mCurrentPos;
}

int DummyFocusT::getTargetPos() const {
	return mTargetPos;
}

void DummyFocusT::setTargetPos(unsigned int inTicks, FocusDirectionT::TypeE direction) {

	LOG(debug) << "DummyFocusT::setTargetPos...inTicks=" << inTicks << ", direction=" << FocusDirectionT::asStr(direction) << std::endl;

	int ticks = inTicks;
	int delta = (direction == FocusDirectionT::INWARD ? -ticks : ticks);

	LOG(debug) << "DummyFocusT::setTargetPos...delta=" << delta << std::endl;

	mTargetPos += delta;

	mMovementRunning = true;
	mMovementAborted = false;
}

void DummyFocusT::setTargetPos(int inAbsPos) {
	mTargetPos = inAbsPos;

	mMovementRunning = true;
	mMovementAborted = false;
}

void DummyFocusT::resetPositionCounter() {
	mTargetPos = 0;
}

int DummyFocusT::getMinAbsPos() const {
	return 0;
}

int DummyFocusT::getMaxAbsPos() const {
	return 32768;
}

bool DummyFocusT::isAbortSupported() const {
	return true;
}

void DummyFocusT::abortMotion() {
	mMovementAborted = true;
}

std::shared_ptr<DeviceConnectorT> DummyFocusT::getConnector() const {
	return mDummyConnector;
}

void DummyFocusT::focusLoop() {
	while(! mCancelFocusLoop) {
		std::this_thread::sleep_for(10ms);

		if (mMovementRunning) {
			if (mTargetPos != mCurrentPos) {
				LOG(debug) << "DummyFocusT::focusLoop... target=" << mTargetPos << ", current=" << mCurrentPos << std::endl;

				// Handle abort of movement
				if (mMovementAborted) {
					mMovementRunning = false;
					mMovementAborted = false;
					notifyFocusMovementAborted(mCurrentPos);

					continue;
				}


				if (mTargetPos < mCurrentPos) {
					mCurrentPos--;
				} else if (mCurrentPos < mTargetPos) {
					mCurrentPos++;
				}
				notifyFocusPositionChanged(mCurrentPos);

				if (mTargetPos == mCurrentPos) {
					notifyTargetPositionReached(mCurrentPos);
				}
			}
		} // end if - mMovementRunning
	}
}
