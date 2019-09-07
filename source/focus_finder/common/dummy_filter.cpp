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

#include "include/dummy_filter.h"


using namespace std::chrono_literals;

DummyFilterT::DummyFilterT() :
	mCancelFilterMovementLoop(false),
	mTargetPos(0),
	mCurrentPos(0),
	mMovementAborted(false),
	mMovementRunning(false) {

	mDummyConnector = std::make_shared<DummyDeviceConnectorT>();

	// Read connection status etc. from driver?

	//	cancelExposureFlag = false;

	filterMovementThread = std::thread(&DummyFilterT::filterMovementLoop, this);
	filterMovementThread.detach();
}

DummyFilterT::~DummyFilterT() {

	// TODO: ATOMIC
	mCancelFilterMovementLoop = true;
}

/////////////////////////////////////////////////
// Device
/////////////////////////////////////////////////
std::string DummyFilterT::getName() const {
	return "MyDummyFilterName";
}

/**
* Generic
*/
bool DummyFilterT::isMoving() const {
	return mMovementRunning;
}

int DummyFilterT::getCurrentPos() const {
	return mCurrentPos;
}

int DummyFilterT::getTargetPos() const {
	return mTargetPos;
}

void DummyFilterT::setTargetPos(int pos) {
	LOG(debug) << "DummyFilterT::setTargetPos..." << std::endl;

	mTargetPos = pos;

	mMovementRunning = true;
	mMovementAborted = false;
}

int DummyFilterT::getMinPos() const {
	return 0;
}

int DummyFilterT::getMaxPos() const {
	return 5;
}

unsigned int DummyFilterT::getNumSlots() const {
	return getMaxPos();
}

bool DummyFilterT::isAbortSupported() const {
	return false;
}

void DummyFilterT::abortMotion() {
	//mMovementAborted = true;
	// OP not supported...
}

std::shared_ptr<DeviceConnectorT> DummyFilterT::getConnector() const {
	return mDummyConnector;
}

void DummyFilterT::filterMovementLoop() {
	while(! mCancelFilterMovementLoop) {
		std::this_thread::sleep_for(2s);

		if (mMovementRunning) {
			if (mTargetPos != mCurrentPos) {
				LOG(debug) << "DummyFilterT::filterMovementLoop... target=" << mTargetPos << ", current=" << mCurrentPos << std::endl;

				// Handle abort of movement
				if (mMovementAborted) {
					mMovementRunning = false;
					mMovementAborted = false;
					notifyFilterMovementAborted(mCurrentPos);

					continue;
				}


				if (mTargetPos < mCurrentPos) {
					mCurrentPos--;
				} else if (mCurrentPos < mTargetPos) {
					mCurrentPos++;
				}
				notifyFilterPositionChanged(mCurrentPos);

				if (mTargetPos == mCurrentPos) {
					notifyTargetPositionReached(mCurrentPos);
				}
			}
		} // end if - mMovementRunning
	}
}
