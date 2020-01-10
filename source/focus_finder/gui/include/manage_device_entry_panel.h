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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_MANAGE_DEVICE_ENTRY_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_MANAGE_DEVICE_ENTRY_PANEL_H_

// std includes
#include <vector>
#include <memory>
#include <string>

// Qt includes
#include <QWidget>

#include "anim_menu_button.h"

#include "../../common/include/focus_finder_logic.h"
#include "../../common/include/device_connector.h"

namespace Ui {
    class ManageDeviceEntryPanel;
}

class DeviceT;

/**
*  @brief
*/
class ManageDeviceEntryPanelT : public QWidget
{
    Q_OBJECT


public:
    /**
    *  @brief
    *    Constructor
    */
	ManageDeviceEntryPanelT(FocusFinderLogicT & ffl, const std::string & deviceTypeName, const std::string & deviceName);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~ManageDeviceEntryPanelT();

	void setDeviceName(const std::string & deviceName);

signals:
	void deviceConnectingSignal();
	void deviceConnectedSignal();
	void deviceDisconnectingSignal();
	void deviceDisconnectedSignal();
	void connectionStateChanged();

protected slots:
	void onDeviceConnectClicked(bool);


protected:
	const QScopedPointer<Ui::ManageDeviceEntryPanel> m_ui;

private:
	void updateConnectButton();
	void setBtnIcon(const std::string & iconName, const std::string & text);
	void setButtonConnectionState(DeviceConnectionStateT::TypeE btnConnState);

	void updateDevice(const std::string & deviceName);
	void createDeviceConnectButton();

	void onDeviceConnecting();
	void onDeviceConnected();
	void onDeviceDisconnecting();
	void onDeviceDisconnected();

	FocusFinderLogicT & mFfl;

	AnimMenuButtonT * mConnectButton;

	std::shared_ptr<DeviceT> mDevice;

	boost::signals2::connection mDeviceConnectingConnection;
	boost::signals2::connection mDeviceConnectedConnection;
	boost::signals2::connection mDeviceDisconnectedConnection;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_MANAGE_DEVICE_ENTRY_PANEL_H_*/
