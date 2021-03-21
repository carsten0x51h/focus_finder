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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_INDI_DEVICE_MANAGER_DIALOG_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_INDI_DEVICE_MANAGER_DIALOG_H_

#include <memory>

#include <boost/signals2.hpp>

// Qt includes
#include <QHBoxLayout>
#include <QDialog>

#include "anim_menu_button.h"

#include "../../common/include/indi_server_connection_state.h"


class IndiDeviceManagerT;

namespace Ui {
    class IndiDeviceManagerDialog;
}


/**
 *  @brief
 */
class IndiDeviceManagerDialogT : public QDialog {
Q_OBJECT

public:
    /**
     *  @brief
     *    Constructor
     */
    IndiDeviceManagerDialogT(QWidget *parent, std::shared_ptr<IndiDeviceManagerT> indiDeviceManager);

    /**
     *  @brief
     *    Destructor
     */
    virtual ~IndiDeviceManagerDialogT();

signals:

    void indiServerConnectionStateChangedSignal(IndiServerConnectionStateT::TypeE indiServerConnectionState);

    void indiServerConnectionFailedSignal();

protected slots:

protected:
    const QScopedPointer<Ui::IndiDeviceManagerDialog> m_ui;

private:
    void setBtnIcon(const std::string &iconName, const std::string &text);

    void setButtonConnectionState(IndiServerConnectionStateT::TypeE btnConnState);

    static bool isServerInfoValid(const std::string &hostname, const std::string &portStr);

    QHBoxLayout *getIndiControlButtons();

    void createConnectIndiServerButton();

    void onConnectIndiServerClicked();

    void onServerConnectionStateChanged(IndiServerConnectionStateT::TypeE indiServerConnectionState);

    void onServerConnectionFailed();

    void onBtnCloseDialogClicked();

    void onHostnameChanged(const QString &hostname);

    void onPortChanged(const QString &portStr);


    boost::signals2::connection mServerConnectionStateChangedConnection;
    boost::signals2::connection mServerConnectionFailedConnection;

    std::shared_ptr<IndiDeviceManagerT> mIndiDeviceManager;
    AnimMenuButtonT *mConnectIndiServerButton; // TODO: Create a ConnectButtonT class.... this should also hold a ConnectionStateT...
    IndiServerConnectionStateT::TypeE mIndiServerConnectionState;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_INDI_DEVICE_MANAGER_DIALOG_H_*/
