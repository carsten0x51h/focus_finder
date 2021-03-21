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

#include <thread>
#include <pthread.h>


#include <chrono> // TODO: Remove

#include "include/indi_client.h"
#include "include/logging.h"
#include "indiproperty.h"
#include "basedevice.h"


IndiClientT::IndiClientT() {

}

IndiClientT::~IndiClientT() {

}

void IndiClientT::newDevice(INDI::BaseDevice *dp) {
    LOG(debug) << "IndiClientT::newDevice..." << std::endl;
    notifyNewDevice(dp);
}

void IndiClientT::removeDevice(INDI::BaseDevice *dp) {
    LOG(debug) << "IndiClientT::removeDevice..." << std::endl;
    notifyRemoveDevice(dp);
}

void IndiClientT::newProperty(INDI::Property *property) {
    LOG(debug) << "IndiClientT::newProperty... " << property->getName() << ", driver interface mask: "
               << property->getBaseDevice()->getDriverInterface() << std::endl;

    notifyNewProperty(property);
}

void IndiClientT::removeProperty(INDI::Property *property) {
    LOG(debug) << "IndiClientT::removeProperty... " << property->getName() << std::endl;
    notifyRemoveProperty(property);
}

void IndiClientT::newBLOB(IBLOB *bp) {
    LOG(debug) << "IndiClientT::newBLOB..." << std::endl;

    notifyNewBlob(bp);
}

void IndiClientT::newSwitch(ISwitchVectorProperty *svp) {
    LOG(debug) << "IndiClientT::newSwitch... name=" << svp->name << std::endl;
    notifyNewSwitch(svp);
}

void IndiClientT::newNumber(INumberVectorProperty *nvp) {
    //LOG(debug) << "IndiClientT::newNumber... name=" << nvp->name << std::endl;
    notifyNewNumber(nvp);
}

void IndiClientT::newText(ITextVectorProperty *tvp) {
    LOG(debug) << "IndiClientT::newText... name=" << tvp->name << std::endl;
    notifyNewText(tvp);
}

void IndiClientT::newLight(ILightVectorProperty *lvp) {
    LOG(debug) << "IndiClientT::newLight... name=" << lvp->name << std::endl;
    notifyNewLight(lvp);
}

void IndiClientT::newMessage(INDI::BaseDevice *dp, int messageID) {
    std::string msgStr = dp->messageQueue(messageID);
    LOG(debug) << "IndiClientT::newMessage...'" << msgStr << "'" << std::endl;
    notifyNewMessage(dp, messageID);
}


// TODO: We may have to change the connection logic - INDI client API blocks??!
void IndiClientT::serverConnected() {
    LOG(debug) << "IndiClientT::serverConnected..." << std::endl;
    notifyServerConnectionStateChanged(IndiServerConnectionStateT::CONNECTED);
}

void IndiClientT::serverDisconnected(int /*exit_code*/) {
    // TODO: Do we need the exit code?...
    LOG(debug) << "IndiClientT::serverDisconnected..." << std::endl;
    notifyServerConnectionStateChanged(IndiServerConnectionStateT::DISCONNECTED);
}

void IndiClientT::connectToIndiServerBlocking() {

    LOG(debug) << "IndiClientT::connectToIndiServerBlocking()... Trying to connecto to INDI server... blocking call..."
               << std::endl;

    // This function is blocking and therefore needs to be in a separate thread
    bool connectIndiServerResult = this->connectServer();

    LOG(debug) << "IndiClientT::connectToIndiServerBlocking()... connectIndiServerResult=" << connectIndiServerResult
               << std::endl;

    if (!connectIndiServerResult) {
        // Emit a failure signal...
        LOG(debug) << "IndiClientT::connectToIndiServerBlocking()... INDI server connection failed..." << std::endl;

        notifyServerConnectionFailed();
    } else {
        // Do not emit a success signal since this will already happen from within the INDI server...
    }
}

void IndiClientT::connect() {
    LOG(debug) << "IndiClientT::connect..." << std::endl;

    // TODO: Use setConnectionTimeout(uint32_t seconds, uint32_t microseconds) from base class to set the INDI server connection timeout (default is 3 seconds).

    if (!this->isServerConnected()) {
        notifyServerConnectionStateChanged(IndiServerConnectionStateT::CONNECTING);

        mIndiConnectServerThread = std::move(std::thread(&IndiClientT::connectToIndiServerBlocking, this));
        mIndiConnectServerThread.detach();
    }
}

void IndiClientT::disconnect() {
    LOG(debug) << "IndiClientT::disconnect..." << std::endl;

    notifyServerConnectionStateChanged(IndiServerConnectionStateT::DISCONNECTING);

    if (this->isServerConnected()) {
        this->disconnectServer();
    }

}

bool IndiClientT::isConnected() const {
    return this->isServerConnected();
}

INDI::BaseDevice *IndiClientT::getDevice(const std::string &deviceName) const {
    return const_cast<INDI::BaseClient *>((const INDI::BaseClient *) this)->getDevice(deviceName.c_str());
}