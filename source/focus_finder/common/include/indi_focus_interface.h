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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INDI_FOCUS_INTERFACE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INDI_FOCUS_INTERFACE_H_

#include <memory>

#include <boost/signals2.hpp>

#include "focus_interface.h"
#include "logging.h"
#include "device_connector.h"
#include "indi_device.h"
#include "indi_client.h"

// INDI
#include "basedevice.h"

// https://stackoverflow.com/questions/9404884/implementing-interfaces-in-c-with-inherited-concrete-classes
class IndiFocusInterfaceT : virtual public FocusInterfaceT {

public:
	IndiFocusInterfaceT(INDI::BaseDevice *dp, IndiClientT * indiClient);
	virtual ~IndiFocusInterfaceT();

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
	//       class of IndiDeviceT and IndiEthernetDeviceConnectorT.
	std::shared_ptr<IndiDeviceT> mIndiConnector;

	void newNumber(INumberVectorProperty * nvp);

	INDI::BaseDevice *mIndiBaseDevice;
	IndiClientT * mIndiClient;

	boost::signals2::connection mNewNumberConnection;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INDI_FOCUS_INTERFACE_H_ */
