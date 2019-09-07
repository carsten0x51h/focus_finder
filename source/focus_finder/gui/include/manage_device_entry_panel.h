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
