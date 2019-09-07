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
	LOG(debug) << "IndiClientT::newProperty... " << property->getName() << std::endl;
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
	notifyServerConnectionStateChanged(true /*connected*/);
}

void IndiClientT::serverDisconnected(int /*exit_code*/) {
	// TODO: Do we need the exit code?...
	LOG(debug) << "IndiClientT::serverDisconnected..." << std::endl;
	notifyServerConnectionStateChanged(false /*disconnected*/);
}
