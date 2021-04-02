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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CNTL_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CNTL_PANEL_H_ SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CNTL_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QSignalMapper>

#include "../../common/include/focus_finder_logic.h"

class FocusInterfaceT;

class QMovie;

namespace Ui {
    class FocusCntlPanel;
}

/**
*  @brief
*/
class FocusCntlPanelT : public QWidget {
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
    FocusCntlPanelT(QWidget *parent, FocusFinderLogicT &ffl);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FocusCntlPanelT();

signals:

    void deviceConnectedSignal();

    void deviceDisconnectedSignal();

    void focusPositionChangedSignal(int absPos);

    void targetPositionReachedSignal(int absPos);

    void focusMovementAbortedSignal(int currentPos);

protected slots:

    void setFocusAnimationIcon(int /*frame*/);

    void onFocusPositionChangeRequest(int inSteps); // One of the +/- buttons pressed
    void onFocusAbortRequest();                     // Abort button pressed

    void onDeviceConnectedSlot();

    void onDeviceDisconnectedSlot();

    void onFocusPositionChangedSlot(int absPos);

    void onTargetPositionReachedSlot(int absPos);

    void onFocusMovementAbortedSlot(int currentPos);

protected:
    const QScopedPointer<Ui::FocusCntlPanel> m_ui;

private:
    void updateCurrentPos(int absPos);

    void updateProfile();

    std::shared_ptr<FocusInterfaceT> getFocus() const;

    void startAnimation();

    void stopAnimation();

    QMovie *mMovie;

    FocusFinderLogicT &mFfl;
    QSignalMapper *mStepSignalMapper;
    std::shared_ptr<FocusInterfaceT> mFocusDevice;

    boost::signals2::connection mDeviceConnectedConnection;
    boost::signals2::connection mDeviceDisconnectedConnection;
    boost::signals2::connection mFocusPositionChangedConnection;
    boost::signals2::connection mTargetPositionReachedConnection;
    boost::signals2::connection mFocusMovementAbortedConnection;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CNTL_PANEL_H_*/
