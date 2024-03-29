/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c [at] lost-infinity.com>
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

#include <memory>

#include "include/indi_helper.h"

#include "include/indi_focus_interface.h"

IndiFocusInterfaceT::IndiFocusInterfaceT(IndiDeviceT *indiDevice) :
        mIndiDevice(indiDevice) {

    LOG(debug) << "IndiFocusInterfaceT::IndiFocusInterfaceT..." << std::endl;

    // Register number
    mNewNumberConnection = mIndiDevice->getIndiClient()->registerNewNumberListener(
            boost::bind(&IndiFocusInterfaceT::newNumber, this, boost::placeholders::_1));
}

IndiFocusInterfaceT::~IndiFocusInterfaceT() {
    LOG(debug) << "IndiFocusInterfaceT::IndiFocusInterfaceTnterfaceT..." << std::endl;

    mIndiDevice->getIndiClient()->unregisterNewNumberListener(mNewNumberConnection);
}

void IndiFocusInterfaceT::newNumber(INumberVectorProperty *nvp) {
    if (strcmp(nvp->device, getName().c_str()) == 0) {

        LOG(debug) << "IndiFocusInterfaceT::newNumber... " << nvp->name
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

            INumber *absPosition = IndiHelperT::getNumber(nvp,
                                                          "FOCUS_ABSOLUTE_POSITION");

            if (nvp->s == IPS_BUSY) {
                notifyFocusPositionChanged((int) absPosition->value);
            } else if (nvp->s == IPS_OK || nvp->s == IPS_IDLE) {
                notifyTargetPositionReached((int) absPosition->value);
            }
        }

        //void notifyFocusMovementAborted(int currentPosition) const { mFocusMovementAbortedListeners(currentPosition); }
    }
}

/////////////////////////////////////////////////
// Device
/////////////////////////////////////////////////
std::string IndiFocusInterfaceT::getName() const {
    return mIndiDevice->getIndiBaseDevice()->getDeviceName();
}

DeviceT *IndiFocusInterfaceT::getParentDevice() {
    return mIndiDevice;
}

/**
 * Temperature
 */
bool IndiFocusInterfaceT::isTemperatureSupported() const {
    //TODO
    return false;
}

float IndiFocusInterfaceT::getTemperature() const {
    // TODO: Throw if not supported?
    // TODO: Add listener - when temperature has changed...
    return 0;
}

/**
 * TODO: Is this a reasonable implementation? Maybe there is a
 *       better way to find out if a focus is currently moving...
 */
bool IndiFocusInterfaceT::isMoving() const {

    bool isRelPosBusy;
    bool isAbsPosBusy;

    try {
        INumberVectorProperty *relPosVecProp = IndiHelperT::getNumberVec(
                mIndiDevice->getIndiBaseDevice(), "REL_FOCUS_POSITION");

        isRelPosBusy = (relPosVecProp->s == IPS_BUSY);

    } catch (IndiExceptionT &exc) {
        isRelPosBusy = false;
    }

    try {
        INumberVectorProperty *absPosVecProp = IndiHelperT::getNumberVec(
                mIndiDevice->getIndiBaseDevice(), "ABS_FOCUS_POSITION");

        isAbsPosBusy = (absPosVecProp->s == IPS_BUSY);

    } catch (IndiExceptionT &exc) {
        isAbsPosBusy = false;
    }

    return (isRelPosBusy || isAbsPosBusy);
}

/**
 * Positioning
 */
bool IndiFocusInterfaceT::isAbsPosSupported() const {
    try {
        INumberVectorProperty *absPosVecProp = IndiHelperT::getNumberVec(
                mIndiDevice->getIndiBaseDevice(), "ABS_FOCUS_POSITION");

        // Just try to get the property...
        IndiHelperT::getNumber(absPosVecProp,"FOCUS_ABSOLUTE_POSITION");

        return true;
    } catch (IndiExceptionT &exc) {
        return false;
    }
}

int IndiFocusInterfaceT::getCurrentPosInternal() const {
    INumberVectorProperty *absPosVecProp = IndiHelperT::getNumberVec(
            mIndiDevice->getIndiBaseDevice(), "ABS_FOCUS_POSITION");

    IndiHelperT::requireReadable(absPosVecProp);

    INumber *absPos = IndiHelperT::getNumber(absPosVecProp,
                                             "FOCUS_ABSOLUTE_POSITION");

    return (int) absPos->value;
}

int IndiFocusInterfaceT::getCurrentPos() const {
    try {
        return getCurrentPosInternal();
    } catch (IndiExceptionT &exc) {
        throw FocusExceptionT(
                "Unable to get current absolute focus position. Details: "
                + std::string(exc.what()));
    }
}

int IndiFocusInterfaceT::getTargetPos() const {
    // TODO: Does the Indi focuser save the target position??
    return 0;
    //return mTargetPos;
}


int IndiFocusInterfaceT::clipTicks(int ticks, FocusDirectionT::TypeE direction) const {

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
void IndiFocusInterfaceT::setTargetPos(unsigned int ticks,
                                       FocusDirectionT::TypeE direction) {

    LOG(debug) << "IndiFocusInterfaceT::setTargetPos...inTicks=" << ticks
               << ", direction=" << FocusDirectionT::asStr(direction) << std::endl;


    try {
        // NOTE: Relative ticks must be greater than 0............
        // To control direction, use switch FOCUS_MOTION -> FOCUS_INWARD, FOCUS_OUTWARD
        ISwitchVectorProperty *focusDirectionVecProp =
                IndiHelperT::getSwitchVec(mIndiDevice->getIndiBaseDevice(), "FOCUS_MOTION");

        IndiHelperT::requireWritable(focusDirectionVecProp);

        ISwitch *focusInward = IndiHelperT::getSwitch(focusDirectionVecProp,
                                                      "FOCUS_INWARD");
        ISwitch *focusOutward = IndiHelperT::getSwitch(focusDirectionVecProp,
                                                       "FOCUS_OUTWARD");

        focusInward->s = (
                direction == FocusDirectionT::INWARD ? ISS_ON : ISS_OFF);
        focusOutward->s = (
                direction == FocusDirectionT::INWARD ? ISS_OFF : ISS_ON);

        mIndiDevice->getIndiClient()->sendNewSwitch(focusDirectionVecProp);

    } catch (IndiExceptionT &exc) {
        throw FocusExceptionT(
                "Unable to set focus direction. Details: "
                + std::string(exc.what()));
    }


    // TODO / IDEA: Always use relative position since this is the minimum property each focuser should have.
    try {

        int ticksClipped = clipTicks((int) ticks, direction);

        if (ticksClipped <= 0) {
            LOG(warning) << "Boundary reached. Not moving." << std::endl;

            int currentPos = getCurrentPosInternal();

            notifyTargetPositionReached(currentPos);

            return;
        }


        INumberVectorProperty *relPosVecProp = IndiHelperT::getNumberVec(
                mIndiDevice->getIndiBaseDevice(), "REL_FOCUS_POSITION");

        IndiHelperT::requireWritable(relPosVecProp);

        INumber *relPos = IndiHelperT::getNumber(relPosVecProp,
                                                 "FOCUS_RELATIVE_POSITION");

        relPos->value = ticksClipped;

        mIndiDevice->getIndiClient()->sendNewNumber(relPosVecProp);

    } catch (IndiExceptionT &exc) {
        throw FocusExceptionT(
                "Unable to set relative position. Details: "
                + std::string(exc.what()));
    }

}

void IndiFocusInterfaceT::setTargetPos(int inAbsPos) {
    try {
        INumberVectorProperty *absPosVecProp = IndiHelperT::getNumberVec(
                mIndiDevice->getIndiBaseDevice(), "ABS_FOCUS_POSITION");

        IndiHelperT::requireWritable(absPosVecProp);

        INumber *absPos = IndiHelperT::getNumber(absPosVecProp,
                                                 "FOCUS_ABSOLUTE_POSITION");

        absPos->value = inAbsPos;

        mIndiDevice->getIndiClient()->sendNewNumber(absPosVecProp);

    } catch (IndiExceptionT &exc) {
        throw FocusExceptionT(
                "Unable to set new absolute focus position. Details: "
                + std::string(exc.what()));
    }
}

void IndiFocusInterfaceT::resetPositionCounter() {
    // TODO
    //mTargetPos = 0;
}

int IndiFocusInterfaceT::getMinAbsPosInternal() const {
    INumberVectorProperty *absPosVecProp = IndiHelperT::getNumberVec(
            mIndiDevice->getIndiBaseDevice(), "ABS_FOCUS_POSITION");

    IndiHelperT::requireReadable(absPosVecProp);

    INumber *absPos = IndiHelperT::getNumber(absPosVecProp,
                                             "FOCUS_ABSOLUTE_POSITION");

    return (int) absPos->min;
}

int IndiFocusInterfaceT::getMinAbsPos() const {
    try {
        return getMinAbsPosInternal();
    } catch (IndiExceptionT &exc) {
        throw FocusExceptionT(
                "Unable to get absolute minimum focus position. Details: "
                + std::string(exc.what()));
    }
}

int IndiFocusInterfaceT::getMaxAbsPosInternal() const {
    INumberVectorProperty *absPosVecProp = IndiHelperT::getNumberVec(
            mIndiDevice->getIndiBaseDevice(), "ABS_FOCUS_POSITION");

    IndiHelperT::requireReadable(absPosVecProp);

    INumber *absPos = IndiHelperT::getNumber(absPosVecProp,
                                             "FOCUS_ABSOLUTE_POSITION");

    return (int) absPos->max;
}

int IndiFocusInterfaceT::getMaxAbsPos() const {
    try {
        return getMaxAbsPosInternal();
    } catch (IndiExceptionT &exc) {
        throw FocusExceptionT(
                "Unable to get absolute minimum focus position. Details: "
                + std::string(exc.what()));
    }
}

bool IndiFocusInterfaceT::isAbortSupported() const {
    // TODO
    return true;
}

void IndiFocusInterfaceT::abortMotion() {
    // TODO
//	mMovementAborted = true;
}
