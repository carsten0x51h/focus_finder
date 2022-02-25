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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INDI_FOCUS_INTERFACE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INDI_FOCUS_INTERFACE_H_

#include <memory>

#include <boost/signals2.hpp>

#include "focus_interface.h"
#include "logging.h"
#include "indi_device.h"
#include "indi_client.h"

#include "basedevice.h"

// https://stackoverflow.com/questions/9404884/implementing-interfaces-in-c-with-inherited-concrete-classes
class IndiFocusInterfaceT : virtual public FocusInterfaceT {

public:
    explicit IndiFocusInterfaceT(IndiDeviceT *indiDevice);

    ~IndiFocusInterfaceT() override;

    [[nodiscard]] std::string getName() const;

    DeviceT *getParentDevice() override;

    // Impl. of focus interface
    [[nodiscard]] bool isTemperatureSupported() const override;

    [[nodiscard]] float getTemperature() const override;

    [[nodiscard]] bool isMoving() const override;

    [[nodiscard]] bool isAbsPosSupported() const override;

    [[nodiscard]] int getCurrentPos() const override;

    [[nodiscard]] int getTargetPos() const override;

    void setTargetPos(unsigned int inTicks, FocusDirectionT::TypeE direction) override;

    void setTargetPos(int inAbsPos) override;

    void resetPositionCounter() override;

    [[nodiscard]] int getMinAbsPos() const override;

    [[nodiscard]] int getMaxAbsPos() const override;

    [[nodiscard]] bool isAbortSupported() const override;

    void abortMotion() override;

private:
    [[nodiscard]] int clipTicks(int ticks, FocusDirectionT::TypeE direction) const;

    [[nodiscard]] int getCurrentPosInternal() const;

    [[nodiscard]] int getMinAbsPosInternal() const;

    [[nodiscard]] int getMaxAbsPosInternal() const;

    void newNumber(INumberVectorProperty *nvp);

    IndiDeviceT *mIndiDevice;

    boost::signals2::connection mNewNumberConnection;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INDI_FOCUS_INTERFACE_H_ */
