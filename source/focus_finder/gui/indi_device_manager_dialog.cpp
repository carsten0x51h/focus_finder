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

// TODO: Add a new button "Reset to defaults" button...

#include "include/indi_device_manager_dialog.h"

#include <utility>

#include "../common/include/logging.h"
#include "../common/include/indi_device_manager.h"

#include "ui_indi_device_manager_dialog.h"


bool IndiDeviceManagerDialogT::isServerInfoValid(const std::string &hostname, const std::string &portStr) {

    bool isHostnameValid = !hostname.empty(); // TODO: May be refined...
    bool isPortValid = !portStr.empty();

    return (isHostnameValid && isPortValid);
}

void IndiDeviceManagerDialogT::onBtnCloseDialogClicked() {
    LOG(debug) << "IndiDeviceManagerDialogT::onBtnCloseDialogClicked()..." << std::endl;

    //done(0);
    close();
}

void IndiDeviceManagerDialogT::onHostnameChanged(const QString &hostname) {
    LOG(debug) << "IndiDeviceManagerDialogT::onHostnameChanged()...new hostname=" << hostname.toStdString()
               << std::endl;

    QString portStr = m_ui->editSpinPort->text();

    mConnectIndiServerButton->setEnabled(isServerInfoValid(
            hostname.toStdString(),
            portStr.toStdString()
                                         )
    );
}

void IndiDeviceManagerDialogT::onPortChanged(const QString &portStr) {
    LOG(debug) << "IndiDeviceManagerDialogT::onPortChanged()...new value=" << portStr.toStdString() << std::endl;

    QString hostname = m_ui->editHostname->text();

    mConnectIndiServerButton->setEnabled(isServerInfoValid(
            hostname.toStdString(),
            portStr.toStdString()
                                         )
    );
}

QHBoxLayout *IndiDeviceManagerDialogT::getIndiControlButtons() {
    return m_ui->layIndiControlButtons;
}


// TODO: Disable the connect button when port or hostname are empty or port is invalid
// TODO: Disable hostname and port edit when connected or connecting or disconnecting

void IndiDeviceManagerDialogT::onConnectIndiServerClicked() {
    LOG(debug) << "IndiDeviceManagerDialogT::onConnectIndiServerClicked()..." << std::endl;

    if (IndiServerConnectionStateT::CONNECTED == mIndiServerConnectionState) {
        LOG(debug) << "Disconnecting from INDI server..." << std::endl;

        setButtonConnectionState(IndiServerConnectionStateT::DISCONNECTING);

        // TODO / NOTE: disconnect() should be an non-blocking call...
        mIndiDeviceManager->getIndiClient().disconnect();
    } else if (IndiServerConnectionStateT::DISCONNECTED == mIndiServerConnectionState) {
        LOG(debug) << "Connecting to INDI server..." << std::endl;

        QString hostname = m_ui->editHostname->text();
        int port = m_ui->editSpinPort->value();

        mIndiDeviceManager->getIndiClient().setServer(hostname.toStdString().c_str(), port);

        mIndiDeviceManager->getIndiClient().connect();
    } else if (IndiServerConnectionStateT::CONNECTING == mIndiServerConnectionState) {
        LOG(debug) << "Connecting cannot be interrupted..." << std::endl;
        // // Indi connectServer() is blocking
        // LOG(debug) << "Cancelling connection attempt... disconnecting..." << std::endl;

        // // Cancel connection attempt...
        // mIndiDeviceManager->getIndiClient().disconnect();
    } else if (IndiServerConnectionStateT::DISCONNECTING == mIndiServerConnectionState) {
        LOG(debug) << "Disconnecting cannot be interrupted..." << std::endl;
    } else {
        LOG(warning) << "Unknown connection state: " << IndiServerConnectionStateT::asStr(mIndiServerConnectionState)
                     << std::endl;
    }
}

void
IndiDeviceManagerDialogT::onServerConnectionStateChanged(IndiServerConnectionStateT::TypeE indiServerConnectionState) {

    LOG(debug) << "IndiDeviceManagerDialogT::onServerConnectionStateChanged(indiServerConnectionState="
               << IndiServerConnectionStateT::asStr(indiServerConnectionState) << ")..." << std::endl;

    setButtonConnectionState(indiServerConnectionState);
}

void IndiDeviceManagerDialogT::onServerConnectionFailed() {
    LOG(debug) << "IndiDeviceManagerDialogT::onServerConnectionFailed()..." << std::endl;

    setButtonConnectionState(IndiServerConnectionStateT::DISCONNECTED);
}

void IndiDeviceManagerDialogT::setBtnIcon(const std::string &iconName,
                                          const std::string &text) {
    QIcon icon;
    icon.addFile(QString::fromStdString(iconName), QSize(), QIcon::Normal,
                 QIcon::Off);
    mConnectIndiServerButton->setIcon(icon);
    mConnectIndiServerButton->setMinimumSize(QSize(130, 32));
    mConnectIndiServerButton->setIconSize(QSize(32, 32));
    mConnectIndiServerButton->setText(QString::fromStdString(text));
}


void IndiDeviceManagerDialogT::setButtonConnectionState(
        IndiServerConnectionStateT::TypeE btnConnState) {

    mIndiServerConnectionState = btnConnState;

    LOG(debug) << "setButtonConnectionState(state=" << IndiServerConnectionStateT::asStr(btnConnState) << ")..."
               << std::endl;

    switch (btnConnState) {
        case IndiServerConnectionStateT::DISCONNECTED: {
            mConnectIndiServerButton->stopAnimation();
            mConnectIndiServerButton->setEnabled(true);

            setBtnIcon(":/res/indi_server_disconnected_64x64.png", "Connect");

            mConnectIndiServerButton->setChecked(false);

            // TODO: Should not be here... -> rename setButtonConnectionState() to setIndiServerConnectionState()...
            m_ui->btnCloseDialog->setEnabled(true);

            break;
        }
        case IndiServerConnectionStateT::CONNECTED: {
            mConnectIndiServerButton->stopAnimation();

            setBtnIcon(":/res/indi_server_connected_64x64.png", "Disconnect");

            mConnectIndiServerButton->setChecked(true);
            mConnectIndiServerButton->setEnabled(true);

            // TODO: Should not be here... -> rename setButtonConnectionState() to setIndiServerConnectionState()...
            m_ui->btnCloseDialog->setEnabled(true);

            break;
        }
        case IndiServerConnectionStateT::CONNECTING: {
            mConnectIndiServerButton->startAnimation();
            mConnectIndiServerButton->setChecked(true);

            /**
             * NOTE: Currently canceling a connection attempt causes problems because
             * libindi connectServer() is blocking and even running it in a thread
             * does not help because this thread cannot be joined or killed. Because
             * of that the thread may still call opConnectFailure() after a while
             * and effect the current button state.
             */
            mConnectIndiServerButton->setEnabled(false);
            mConnectIndiServerButton->setText("Connecting");

            // TODO: Should not be here... -> rename setButtonConnectionState() to setIndiServerConnectionState()...
            m_ui->btnCloseDialog->setEnabled(false);

            break;
        }
        case IndiServerConnectionStateT::DISCONNECTING: {
            mConnectIndiServerButton->startAnimation();
            mConnectIndiServerButton->setChecked(true);
            mConnectIndiServerButton->setEnabled(false);
            mConnectIndiServerButton->setText("Disconnecting");

            // TODO: Should not be here... -> rename setButtonConnectionState() to setIndiServerConnectionState()...
            m_ui->btnCloseDialog->setEnabled(false);

            break;
        }
        default: {
            mConnectIndiServerButton->stopAnimation();
            mConnectIndiServerButton->setEnabled(false);

            setBtnIcon(":/res/indi_server_disconnected_64x64.png", "Connect");

            mConnectIndiServerButton->setChecked(false);
        }
    }
}

void IndiDeviceManagerDialogT::createConnectIndiServerButton() {


    // TODO: Execut this when Connect button is pressed...
    // TODO: Introduce connection state change listeners...
    // TODO: Use button from main_window with different states and animation...
    //   mIndiDeviceManager->connectServer();

    mConnectIndiServerButton = new AnimMenuButtonT;

    mConnectIndiServerButton->setCheckable(true);
    mConnectIndiServerButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    connect(mConnectIndiServerButton, &QToolButton::clicked, this,
            &IndiDeviceManagerDialogT::onConnectIndiServerClicked);

    // Set current connection state
    bool isIndiServerConnected = mIndiDeviceManager->getIndiClient().isConnected();
    setButtonConnectionState(
            isIndiServerConnected ? IndiServerConnectionStateT::CONNECTED : IndiServerConnectionStateT::DISCONNECTED);

    // Connect INDI server events with connect button
    connect(this, &IndiDeviceManagerDialogT::indiServerConnectionStateChangedSignal, this,
            &IndiDeviceManagerDialogT::onServerConnectionStateChanged);

    connect(this, &IndiDeviceManagerDialogT::indiServerConnectionFailedSignal, this,
            &IndiDeviceManagerDialogT::onServerConnectionFailed);


    getIndiControlButtons()->addWidget(mConnectIndiServerButton);
}

IndiDeviceManagerDialogT::IndiDeviceManagerDialogT(QWidget *parent,
                                                   std::shared_ptr<IndiDeviceManagerT> indiDeviceManager) : QDialog(
        parent),
                                                                                                            m_ui(new Ui::IndiDeviceManagerDialog),
                                                                                                            mIndiDeviceManager(std::move(
                                                                                                                    indiDeviceManager)), mConnectIndiServerButton(nullptr), mIndiServerConnectionState(IndiServerConnectionStateT::DISCONNECTED) {
    LOG(debug) << "IndiDeviceManagerDialogT::IndiDeviceManagerDialogT..." << std::endl;

    qRegisterMetaType<IndiServerConnectionStateT::TypeE>("IndiServerConnectionStateT::TypeE");

    // Setup UI
    m_ui->setupUi(this);

    setFixedSize(size());

    // Populate the INDI server fields...
    m_ui->editHostname->setText(QString::fromStdString(mIndiDeviceManager->getHostname()));
    m_ui->editSpinPort->setValue((int) mIndiDeviceManager->getPort());

    createConnectIndiServerButton();



    // Connext hostname line edit field
    connect(m_ui->editHostname, &QLineEdit::textChanged, this,
            &IndiDeviceManagerDialogT::onHostnameChanged);

    // Connect port edit spin box
    connect(m_ui->editSpinPort, &QSpinBox::textChanged, this,
            &IndiDeviceManagerDialogT::onPortChanged);

    // Connect dialog close buttoin
    connect(m_ui->btnCloseDialog, &QPushButton::clicked, this,
            &IndiDeviceManagerDialogT::onBtnCloseDialogClicked);


    // TODO: Should this remain here?
    mServerConnectionStateChangedConnection =
            mIndiDeviceManager->getIndiClient().registerServerConnectionStateChangedListener(
                    [&](IndiServerConnectionStateT::TypeE indiServerConnectionState) {
                        LOG(debug) << "IndiServerConnectionStateChanged handler..." << std::endl;
                        emit indiServerConnectionStateChangedSignal(indiServerConnectionState);
                    });

    mServerConnectionFailedConnection =
            mIndiDeviceManager->getIndiClient().registerServerConnectionFailedListener([&]() {
                emit indiServerConnectionFailedSignal();
            });

}

IndiDeviceManagerDialogT::~IndiDeviceManagerDialogT() {
    LOG(debug) << "IndiDeviceManagerDialogT::~IndiDeviceManagerDialogT..." << std::endl;

    // TODO: unregister... mServerConnectionStateChangedConnection ...
    // TODO: unregister... mServerConnectionFailedConnection ...
}
