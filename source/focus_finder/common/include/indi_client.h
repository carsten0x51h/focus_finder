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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_CLIENT_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_CLIENT_H_

#include "logging.h"

#include "indiapi.h"
//#include "indidevapi.h"
//#include "indicom.h"
#include "baseclient.h"
//#include "basedevice.h"

#include <boost/signals2.hpp>

class IndiClientT : public INDI::BaseClient {
private:

	typedef boost::signals2::signal<void (INDI::BaseDevice *dp)> NewDeviceListenersT;
	NewDeviceListenersT mNewDeviceListeners;

	typedef boost::signals2::signal<void (INDI::BaseDevice *dp)> RemoveDeviceListenersT;
	RemoveDeviceListenersT mRemoveDeviceListeners;

	typedef boost::signals2::signal<void (INDI::Property *property)> NewPropertyListenersT;
	NewPropertyListenersT mNewPropertyListeners;

	typedef boost::signals2::signal<void (INDI::Property *property)> RemovePropertyListenersT;
	RemovePropertyListenersT mRemovePropertyListeners;

	typedef boost::signals2::signal<void (INumberVectorProperty *nvp)> NewNumberListenersT;
	NewNumberListenersT mNewNumberListeners;

	typedef boost::signals2::signal<void (ITextVectorProperty *tvp)> NewTextListenersT;
	NewTextListenersT mNewTextListeners;

	typedef boost::signals2::signal<void (ISwitchVectorProperty *svp)> NewSwitchListenersT;
	NewSwitchListenersT mNewSwitchListeners;

	typedef boost::signals2::signal<void (ILightVectorProperty *lvp)> NewLightListenersT;
	NewLightListenersT mNewLightListeners;

	typedef boost::signals2::signal<void (IBLOB *bp)> NewBlobListenersT;
	NewBlobListenersT mNewBlobListeners;

	typedef boost::signals2::signal<void (INDI::BaseDevice *dp, int messageID)> NewMessageListenersT;
	NewMessageListenersT mNewMessageListeners;

	typedef boost::signals2::signal<void (bool connected)> ServerConnectionStateChangedListenersT;
	ServerConnectionStateChangedListenersT mServerConectionStateChangedListeners;

	// We do not want device copies
	IndiClientT(const IndiClientT &);
	IndiClientT & operator=(const IndiClientT &);

public:
	IndiClientT();
	virtual ~IndiClientT();

	boost::signals2::connection registerNewPropertyListener(const NewPropertyListenersT::slot_type & inCallBack) {
		return mNewPropertyListeners.connect(inCallBack);
	}

	template <class T> void unregisterNewPropertyListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

	boost::signals2::connection registerRemovePropertyListener(const RemovePropertyListenersT::slot_type & inCallBack) {
		return mRemovePropertyListeners.connect(inCallBack);
	}

	template <class T> void unregisterRemovePropertyListener(const T & inCallBack) {
		inCallBack.disconnect();
	}


	boost::signals2::connection registerRemoveDeviceListener(const RemoveDeviceListenersT::slot_type & inCallBack) {
		return mRemoveDeviceListeners.connect(inCallBack);
	}

	template <class T> void unregisterRemoveDeviceListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

	boost::signals2::connection registerNewDeviceListener(const NewDeviceListenersT::slot_type & inCallBack) {
		return mNewDeviceListeners.connect(inCallBack);
	}

	template <class T> void unregisterNewDeviceListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

	boost::signals2::connection registerNewNumberListener(const NewNumberListenersT::slot_type & inCallBack) {
		return mNewNumberListeners.connect(inCallBack);
	}

	template <class T> void unregisterNewNumberListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

	boost::signals2::connection registerNewTextListener(const NewTextListenersT::slot_type & inCallBack) {
		return mNewTextListeners.connect(inCallBack);
	}

	template <class T> void unregisterNewTextListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

	boost::signals2::connection registerNewSwitchListener(const NewSwitchListenersT::slot_type & inCallBack) {
		return mNewSwitchListeners.connect(inCallBack);
	}

	template <class T> void unregisterNewSwitchListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

	boost::signals2::connection registerNewLightListener(const NewLightListenersT::slot_type & inCallBack) {
		return mNewLightListeners.connect(inCallBack);
	}

	template <class T> void unregisterNewLightListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

	boost::signals2::connection registerNewBlobListener(const NewBlobListenersT::slot_type & inCallBack) {
		return mNewBlobListeners.connect(inCallBack);
	}

	template <class T> void unregisterNewBlobListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

	boost::signals2::connection registerNewMessageListener(const NewMessageListenersT::slot_type & inCallBack) {
		return mNewMessageListeners.connect(inCallBack);
	}

	template <class T> void unregisterNewMessageListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

	boost::signals2::connection registerServerConnectionStateChangedListener(const ServerConnectionStateChangedListenersT::slot_type & inCallBack) {
		return mServerConectionStateChangedListeners.connect(inCallBack);
	}

	template <class T> void unregisterServerConnectionStateChangedListener(const T & inCallBack) {
		inCallBack.disconnect();
	}


protected:
	void notifyNewDevice(INDI::BaseDevice *dp) { mNewDeviceListeners(dp); }
	void notifyRemoveDevice(INDI::BaseDevice *dp) { mRemoveDeviceListeners(dp); }

	void notifyNewProperty(INDI::Property *property) { mNewPropertyListeners(property); }
	void notifyRemoveProperty(INDI::Property *property) { mRemovePropertyListeners(property); }

	void notifyNewNumber(INumberVectorProperty *nvp) { mNewNumberListeners(nvp); }
	void notifyNewText(ITextVectorProperty *tvp) { mNewTextListeners(tvp); }
	void notifyNewSwitch(ISwitchVectorProperty *svp) { mNewSwitchListeners(svp); }
	void notifyNewLight(ILightVectorProperty *lvp) { mNewLightListeners(lvp); }
	void notifyNewBlob(IBLOB *bp) { mNewBlobListeners(bp); }
	void notifyNewMessage(INDI::BaseDevice *dp, int messageID) { mNewMessageListeners(dp, messageID); }
	void notifyServerConnectionStateChanged(bool connected) { mServerConectionStateChangedListeners(connected); }


	/////////////////////////////////////////////////////////
	// Implement the base device methods
	/////////////////////////////////////////////////////////
	void newDevice(INDI::BaseDevice *dp);
    void removeDevice(INDI::BaseDevice *dp);
    void newProperty(INDI::Property *property);
    void removeProperty(INDI::Property *property);
    void newBLOB(IBLOB *bp);
    void newSwitch(ISwitchVectorProperty *svp);
    void newNumber(INumberVectorProperty *nvp);
    void newText(ITextVectorProperty *tvp);
    void newLight(ILightVectorProperty *lvp);
    void newMessage(INDI::BaseDevice *dp, int messageID);
    void serverConnected();
    void serverDisconnected(int exit_code);
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_CLIENT_H_ */
