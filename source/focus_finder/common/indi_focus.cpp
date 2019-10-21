/*
 * dummy_focus.cpp
 *
 *  Created on: Apr 20, 2019
 *      Author: devnull
 */

#include <memory>

#include "include/indi_helper.h"

#include "include/indi_focus.h"

IndiFocusT::IndiFocusT(INDI::BaseDevice *dp, IndiClientT * indiClient) :
		mIndiBaseDevice(dp), mIndiClient(indiClient) {

	LOG(debug) << "IndiFocusT::IndiFocusT..." << std::endl;

	mDeviceType = (DeviceTypeT::FOCUS);

	mIndiConnector = std::make_shared < IndiUsbDeviceConnectorT
			> (dp, indiClient);

	// Register number
	mNewNumberConnection = mIndiClient->registerNewNumberListener(
			boost::bind(&IndiFocusT::newNumber, this, _1));
}

IndiFocusT::~IndiFocusT() {
	LOG(debug) << "IndiFocusT::~IndiFocusT..." << std::endl;

	mIndiClient->unregisterNewNumberListener(mNewNumberConnection);
}

void IndiFocusT::newNumber(INumberVectorProperty * nvp) {
	if (strcmp(nvp->device, getName().c_str()) == 0) {

		LOG(debug) << "IndiFocusT::newNumber... " << nvp->name
				<< " has changed..., status="
				<< IndiHelperT::propStateAsStr(nvp->s) << std::endl;

		// TODO: Handle ? -> Are we interested in any number update? (especially status changes?!!!)
		// TODO: Yes... when focus position has changed -> update listener....
		//
		// TODO: Should we send update in case of both changes - rel and abs??
//		if (strcmp(nvp->name, "REL_FOCUS_POSITION") == 0) {
//			INumber * relPosition = IndiHelperT::getNumber(nvp,
//					"FOCUS_RELATIVE_POSITION");
//
//			if (nvp->s == IPS_BUSY) {
//				notifyFocusPositionChanged(relPosition->value);
//			} else if (nvp->s == IPS_OK || nvp->s == IPS_IDLE) {
//				notifyTargetPositionReached(relPosition->value);
//			}
//		}
		if (strcmp(nvp->name, "ABS_FOCUS_POSITION") == 0) {

			INumber * absPosition = IndiHelperT::getNumber(nvp,
					"FOCUS_ABSOLUTE_POSITION");

			if (nvp->s == IPS_BUSY) {
				notifyFocusPositionChanged(absPosition->value);
			} else if (nvp->s == IPS_OK || nvp->s == IPS_IDLE) {
				notifyTargetPositionReached(absPosition->value);
			}
		}

		//void notifyFocusMovementAborted(int currentPosition) const { mFocusMovementAbortedListeners(currentPosition); }
	}
}

/////////////////////////////////////////////////
// Device
/////////////////////////////////////////////////
std::string IndiFocusT::getName() const {
	return mIndiBaseDevice->getDeviceName();
}

std::shared_ptr<DeviceConnectorT> IndiFocusT::getConnector() const {
	return mIndiConnector;
}

/**
 * Temperature
 */
bool IndiFocusT::isTemperatureSupported() const {
	//TODO
	return false;
}

float IndiFocusT::getTemperature() const {
	// TODO: Throw if not supported?
	// TODO: Add listener - when temperature has changed...
	return 0;
}

/**
 * TODO: Is this a reasonable implementation? Maybe there is a
 *       better way to find out if a focus is currently moving...
 */
bool IndiFocusT::isMoving() const {

  bool isRelPosBusy;
  bool isAbsPosBusy;

	try {
		INumberVectorProperty * relPosVecProp = IndiHelperT::getNumberVec(
				mIndiBaseDevice, "REL_FOCUS_POSITION");

		isRelPosBusy = (relPosVecProp->s == IPS_BUSY);

	} catch (IndiExceptionT & exc) {
		isRelPosBusy = false;
	}

	try {
		INumberVectorProperty * absPosVecProp = IndiHelperT::getNumberVec(
				mIndiBaseDevice, "ABS_FOCUS_POSITION");

		isAbsPosBusy = (absPosVecProp->s == IPS_BUSY);

	} catch (IndiExceptionT & exc) {
		isAbsPosBusy = false;
	}

	return (isRelPosBusy || isAbsPosBusy);
}

/**
 * Positioning
 */
bool IndiFocusT::isAbsPosSupported() const {
	try {
		INumberVectorProperty * absPosVecProp = IndiHelperT::getNumberVec(
				mIndiBaseDevice, "ABS_FOCUS_POSITION");

		INumber * absPos = IndiHelperT::getNumber(absPosVecProp,
				"FOCUS_ABSOLUTE_POSITION");

		return true;
	} catch (IndiExceptionT & exc) {
		return false;
	}
}

int IndiFocusT::getCurrentPosInternal() const {
	INumberVectorProperty * absPosVecProp = IndiHelperT::getNumberVec(
			mIndiBaseDevice, "ABS_FOCUS_POSITION");

	IndiHelperT::requireReadable(absPosVecProp);

	INumber * absPos = IndiHelperT::getNumber(absPosVecProp,
			"FOCUS_ABSOLUTE_POSITION");

	return absPos->value;
}

int IndiFocusT::getCurrentPos() const {
	try {
		return getCurrentPosInternal();
	} catch (IndiExceptionT & exc) {
		throw FocusExceptionT(
				"Unable to get current absolute focus position. Details: "
						+ std::string(exc.what()));
	}
}

int IndiFocusT::getTargetPos() const {
	// TODO: Does the Indi focuser save the target position??
	return 0;
	//return mTargetPos;
}


int IndiFocusT::clipTicks(int ticks, FocusDirectionT::TypeE direction) const {

	// TODO - sep. function - clipPosDelta()...?
	int minAbsPos = getMinAbsPosInternal();
	int maxAbsPos = getMaxAbsPosInternal();
	int currentPos = getCurrentPosInternal();

	// Need to clip the value in to avoid hitting the boundary.
	int ticksClipped = 0;

	if (direction == FocusDirectionT::INWARD) {

		int possibleTicks = (currentPos - minAbsPos);

		ticksClipped = (
				possibleTicks < (int) ticks ? possibleTicks : ticks);
	} else if (direction == FocusDirectionT::OUTWARD) {
		int possibleTicks = (maxAbsPos - currentPos);

		ticksClipped = (
				possibleTicks < (int) ticks ? possibleTicks : ticks);
	}

	LOG(debug) << "CurrentPos:" << currentPos << ", maxAbsPos: "
			<< maxAbsPos << ", minAbsPos: " << minAbsPos
			<< ", ticksClipped: " << ticksClipped << std::endl;

	return ticksClipped;
}


// TODO: Maybe a better name to indicate "delta" / "relative" movement...
void IndiFocusT::setTargetPos(unsigned int ticks,
		FocusDirectionT::TypeE direction) {

	LOG(debug) << "IndiFocusT::setTargetPos...inTicks=" << ticks
			<< ", direction=" << FocusDirectionT::asStr(direction) << std::endl;


	try {
		// NOTE: Relative ticks must be greater than 0............
		// To control direction, use switch FOCUS_MOTION -> FOCUS_INWARD, FOCUS_OUTWARD
		ISwitchVectorProperty * focusDirectionVecProp =
				IndiHelperT::getSwitchVec(mIndiBaseDevice, "FOCUS_MOTION");

		IndiHelperT::requireWritable(focusDirectionVecProp);

		ISwitch * focusInward = IndiHelperT::getSwitch(focusDirectionVecProp,
				"FOCUS_INWARD");
		ISwitch * focusOutward = IndiHelperT::getSwitch(focusDirectionVecProp,
				"FOCUS_OUTWARD");

		focusInward->s = (
				direction == FocusDirectionT::INWARD ? ISS_ON : ISS_OFF);
		focusOutward->s = (
				direction == FocusDirectionT::INWARD ? ISS_OFF : ISS_ON);

		mIndiClient->sendNewSwitch(focusDirectionVecProp);

	} catch (IndiExceptionT & exc) {
		throw FocusExceptionT(
				"Unable to set focus direction. Details: "
						+ std::string(exc.what()));
	}


	// TODO / IDEA: Always use relative position since this is the minimum property each focuser should have.
	try {

		int ticksClipped = clipTicks(ticks, direction);

		if (ticksClipped <= 0) {
			LOG(warning) << "Boundary reached. Not moving." << std::endl;

			int currentPos = getCurrentPosInternal();

			notifyTargetPositionReached(currentPos);

			return;
		}


		INumberVectorProperty * relPosVecProp = IndiHelperT::getNumberVec(
				mIndiBaseDevice, "REL_FOCUS_POSITION");

		IndiHelperT::requireWritable(relPosVecProp);

		INumber * relPos = IndiHelperT::getNumber(relPosVecProp,
				"FOCUS_RELATIVE_POSITION");

		relPos->value = ticksClipped;

		mIndiClient->sendNewNumber(relPosVecProp);

	} catch (IndiExceptionT & exc) {
		throw FocusExceptionT(
				"Unable to set relative position. Details: "
						+ std::string(exc.what()));
	}

}

void IndiFocusT::setTargetPos(int inAbsPos) {
	try {
		INumberVectorProperty * absPosVecProp = IndiHelperT::getNumberVec(
				mIndiBaseDevice, "ABS_FOCUS_POSITION");

		IndiHelperT::requireWritable(absPosVecProp);

		INumber * absPos = IndiHelperT::getNumber(absPosVecProp,
				"FOCUS_ABSOLUTE_POSITION");

		absPos->value = inAbsPos;

		mIndiClient->sendNewNumber(absPosVecProp);

	} catch (IndiExceptionT & exc) {
		throw FocusExceptionT(
				"Unable to set new absolute focus position. Details: "
						+ std::string(exc.what()));
	}
}

void IndiFocusT::resetPositionCounter() {
	// TODO
	//mTargetPos = 0;
}

int IndiFocusT::getMinAbsPosInternal() const {
	INumberVectorProperty * absPosVecProp = IndiHelperT::getNumberVec(
			mIndiBaseDevice, "ABS_FOCUS_POSITION");

	IndiHelperT::requireReadable(absPosVecProp);

	INumber * absPos = IndiHelperT::getNumber(absPosVecProp,
			"FOCUS_ABSOLUTE_POSITION");

	return absPos->min;
}

int IndiFocusT::getMinAbsPos() const {
	try {
		return getMinAbsPosInternal();
	} catch (IndiExceptionT & exc) {
		throw FocusExceptionT(
				"Unable to get absolute minimum focus position. Details: "
						+ std::string(exc.what()));
	}
}

int IndiFocusT::getMaxAbsPosInternal() const {
	INumberVectorProperty * absPosVecProp = IndiHelperT::getNumberVec(
			mIndiBaseDevice, "ABS_FOCUS_POSITION");

	IndiHelperT::requireReadable(absPosVecProp);

	INumber * absPos = IndiHelperT::getNumber(absPosVecProp,
			"FOCUS_ABSOLUTE_POSITION");

	return absPos->max;
}

int IndiFocusT::getMaxAbsPos() const {
	try {
		return getMaxAbsPosInternal();
	} catch (IndiExceptionT & exc) {
		throw FocusExceptionT(
				"Unable to get absolute minimum focus position. Details: "
						+ std::string(exc.what()));
	}
}

bool IndiFocusT::isAbortSupported() const {
	// TODO
	return true;
}

void IndiFocusT::abortMotion() {
	// TODO
//	mMovementAborted = true;
}
