/*
 * indi_camera.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INDI_FOCUS_H_
#define SOURCE_FOCUS_FINDER_COMMON_INDI_FOCUS_H_

#include <memory>

#include <boost/signals2.hpp>

#include "focus.h"
#include "logging.h"
#include "device_connector.h"
#include "indi_usb_device_connector.h"
#include "indi_client.h"

// INDI
#include "basedevice.h"

// https://stackoverflow.com/questions/9404884/implementing-interfaces-in-c-with-inherited-concrete-classes
class IndiFocusT : virtual public FocusT {

public:
	IndiFocusT(INDI::BaseDevice *dp, IndiClientT * indiClient);
	virtual ~IndiFocusT();

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
	int clipTicks(int ticks, FocusDirectionT::TypeE direction) const;
	int getCurrentPosInternal() const;
	int getMinAbsPosInternal() const;
	int getMaxAbsPosInternal() const;

	// TODO: This will probably later be IndiDeviceConnectorT - a super
	//       class of IndiUsbDeviceConnectorT and IndiEthernetDeviceConnectorT.
	std::shared_ptr<IndiUsbDeviceConnectorT> mIndiConnector;

	void newNumber(INumberVectorProperty * nvp);

	INDI::BaseDevice *mIndiBaseDevice;
	IndiClientT * mIndiClient;

	boost::signals2::connection mNewNumberConnection;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INDI_FOCUS_H_ */
