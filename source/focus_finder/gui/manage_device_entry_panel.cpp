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

#include <memory>

#include "include/anim_menu_button.h"
#include "include/manage_device_entry_panel.h"

#include "../common/include/device_manager.h"

#include "ui_manage_device_entry_panel.h"

// Called when set from "outside" (e.g. when diffferent profile has been selected)
void ManageDeviceEntryPanelT::setDeviceName(const std::string &deviceName) {
    m_ui->lblDeviceName->setText(QString::fromStdString(deviceName));

    updateDevice(deviceName);
    updateConnectButton();
}

void ManageDeviceEntryPanelT::updateDevice(const std::string &deviceName) {

    LOG(debug) << "ManageDeviceEntryPanelT::updateDevice... deviceName=" << deviceName << std::endl;

    auto newDevice = mFfl.getDeviceManager()->getDevice(deviceName);
    bool hasOldDevice = (mDevice != nullptr);

    if (mDevice == newDevice) {
        LOG(debug) << "ManageDeviceEntryPanelT::updateDevice... device has not changed." << std::endl;
        return;
    }

    if (hasOldDevice) {
        LOG(debug) << "ManageDeviceEntryPanelT::updateDevice... unregistering from old device." << std::endl;

        // There was already an old device - unregister listener and register to the new one.
        mDevice->unregisterDeviceConnectingListener(mDeviceConnectingConnection);
        mDevice->unregisterDeviceConnectedListener(mDeviceConnectedConnection);
        mDevice->unregisterDeviceDisconnectedListener(mDeviceDisconnectedConnection);
    }

    // Register to new device
    if (newDevice) {
        LOG(debug) << "ManageDeviceEntryPanelT::updateDevice... registering to new device." << std::endl;

        mDeviceConnectingConnection =
                newDevice->registerDeviceConnectingListener(
                        [&]() { emit deviceConnectingSignal(); });
        mDeviceConnectedConnection =
                newDevice->registerDeviceConnectedListener(
                        [&]() { emit deviceConnectedSignal(); });
        mDeviceDisconnectedConnection =
                newDevice->registerDeviceDisconnectedListener(
                        [&]() { emit deviceDisconnectedSignal(); });
    }

    mDevice = newDevice;
}

void ManageDeviceEntryPanelT::onDeviceConnecting() {
    LOG(debug)
        << "ManageDeviceEntryPanelT::onDeviceConnecting()..." << std::endl;

    setButtonConnectionState(DeviceConnectionStateT::CONNECTING);

    emit connectionStateChanged();
}

void ManageDeviceEntryPanelT::onDeviceConnected() {
    LOG(debug)
        << "ManageDeviceEntryPanelT::onDeviceConnected()..." << std::endl;

    setButtonConnectionState(DeviceConnectionStateT::CONNECTED);

    emit connectionStateChanged();
}

void ManageDeviceEntryPanelT::onDeviceDisconnecting() {
    LOG(debug)
        << "ManageDeviceEntryPanelT::onDeviceDisconnecting()..." << std::endl;

    setButtonConnectionState(DeviceConnectionStateT::DISCONNECTING);

    emit connectionStateChanged();
}

void ManageDeviceEntryPanelT::onDeviceDisconnected() {
    LOG(debug)
        << "ManageDeviceEntryPanelT::onDeviceDisconnected()..." << std::endl;

    setButtonConnectionState(DeviceConnectionStateT::DISCONNECTED);

    emit connectionStateChanged();
}

void ManageDeviceEntryPanelT::onDeviceConnectClicked(bool checked) {
    LOG(debug)
        << "onDeviceConnectClicked() - mConnectButton->isChecked(): " << checked
        << std::endl;

    if (!checked) {
        LOG(debug)
            << "Cancel connect / disconnect ... / " << std::endl;
        mConnectButton->setEnabled(false);
        mDevice->disconnect();
    } else {
        LOG(debug)
            << "Connect ..." << std::endl;
        mDevice->connect();
    }
}

void ManageDeviceEntryPanelT::updateConnectButton() {

    LOG(debug) << "ManageDeviceEntryPanelT::updateConnectButton... device="
               << (mDevice == nullptr ? "<not set>" : mDevice->getName()) << std::endl;

    bool hasDevice = (mDevice != nullptr);

    if (hasDevice) {
        setButtonConnectionState(mDevice->getConnectionState());
        mConnectButton->setEnabled(true);
    } else {
        setButtonConnectionState(DeviceConnectionStateT::DISCONNECTED);
        mConnectButton->setEnabled(false);
    }
}

void ManageDeviceEntryPanelT::setBtnIcon(const std::string &iconName,
                                         const std::string &text) {
    QIcon icon;
    icon.addFile(QString::fromStdString(iconName), QSize(), QIcon::Normal,
                 QIcon::Off);
    mConnectButton->setIcon(icon);
    mConnectButton->setMinimumSize(QSize(130, 32));
    mConnectButton->setIconSize(QSize(32, 32));
    mConnectButton->setText(QString::fromStdString(text));
}

void ManageDeviceEntryPanelT::setButtonConnectionState(
        DeviceConnectionStateT::TypeE btnConnState) {

    switch (btnConnState) {
        case DeviceConnectionStateT::DISCONNECTED: {
            mConnectButton->stopAnimation();
            mConnectButton->setEnabled(true);

            setBtnIcon(":/res/device_disconnected_64x64.png", "Connect");

            mConnectButton->setChecked(false);
            break;
        }
        case DeviceConnectionStateT::CONNECTED: {
            mConnectButton->stopAnimation();

            setBtnIcon(":/res/device_connected_64x64.png", "Disconnect");

            mConnectButton->setChecked(true);

            break;
        }
        case DeviceConnectionStateT::CONNECTING: {
            mConnectButton->startAnimation();
            mConnectButton->setChecked(true);
            mConnectButton->setText("Connecting");
            break;
        }
        case DeviceConnectionStateT::DISCONNECTING: {
            mConnectButton->startAnimation();
            mConnectButton->setChecked(true);
            mConnectButton->setEnabled(false);
            mConnectButton->setText("Disconnecting");
            break;
        }
        default: {
            mConnectButton->stopAnimation();
            mConnectButton->setEnabled(false);

            setBtnIcon(":/res/device_disconnected_64x64.png", "Connect");

            mConnectButton->setChecked(false);
        }
    }
}

void ManageDeviceEntryPanelT::createDeviceConnectButton() {
    mConnectButton = new AnimMenuButtonT;
    mConnectButton->setCheckable(true);
    mConnectButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    mConnectButton->setEnabled(mDevice != nullptr);

    setBtnIcon(":/res/device_disconnected_64x64.png", "Connect");

    connect(mConnectButton, &QToolButton::clicked, this,
            &ManageDeviceEntryPanelT::onDeviceConnectClicked);

    // Connect " connecting" event to " connect button"
    connect(this, &ManageDeviceEntryPanelT::deviceConnectingSignal, this,
            &ManageDeviceEntryPanelT::onDeviceConnecting);

    // Connect " connected" event to " connect button"
    connect(this, &ManageDeviceEntryPanelT::deviceConnectedSignal, this,
            &ManageDeviceEntryPanelT::onDeviceConnected);

    connect(this, &ManageDeviceEntryPanelT::deviceDisconnectingSignal, this,
            &ManageDeviceEntryPanelT::onDeviceDisconnecting);

    // Connect " connect cancelled -> disconnected" event to " connect button"
    connect(this, &ManageDeviceEntryPanelT::deviceDisconnectedSignal, this,
            &ManageDeviceEntryPanelT::onDeviceDisconnected);

    // Connect " cancel button" callback event
    //notifyDeviceConnecting()

    // TODO: Register on cancel event: registerDeviceConnectCancelledListener()....
    // TODO: Register on timeout event
    // TODO: Register on disconnect event

    m_ui->layDevice->addWidget(mConnectButton, 0, 2, 1, 1);
}

ManageDeviceEntryPanelT::ManageDeviceEntryPanelT(FocusFinderLogicT &ffl,
                                                 const std::string &deviceTypeName, const std::string &deviceName) : m_ui(new Ui::ManageDeviceEntryPanel), mFfl(ffl), mConnectButton(nullptr) {
    // Setup UI
    m_ui->setupUi(this);

    mDevice = mFfl.getDeviceManager()->getDevice(deviceName);

    // Set device type name and name
    m_ui->lblDeviceTypeName->setText(QString::fromStdString(deviceTypeName));
    m_ui->lblDeviceName->setText(QString::fromStdString(deviceName));

    //initDeviceComboBox(deviceList, selectedDeviceName);
    createDeviceConnectButton();

    updateConnectButton();
}

ManageDeviceEntryPanelT::~ManageDeviceEntryPanelT() {
    delete mConnectButton;
}
