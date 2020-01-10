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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FILTER_CNTL_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FILTER_CNTL_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QSignalMapper>

#include "../../common/include/focus_finder_logic.h"

class FilterT;
class QMovie;

namespace Ui {
    class FilterCntlPanel;
}

/**
*  @brief
*/
class FilterCntlPanelT : public QWidget
{
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
	FilterCntlPanelT(QWidget * parent, FocusFinderLogicT & ffl);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FilterCntlPanelT();

signals:
	void deviceConnectedSignal();
	void deviceDisconnectedSignal();
	void filterPositionChangedSignal(int currentPos);
	void targetPositionReachedSignal(int targetPos);
	void filterMovementAbortedSignal(int currentPos);

protected slots:
	void setFilterAnimationIcon(int /*frame*/);
	void onFilterPositionChangeRequest(int targetIdx);
//	void onFocusAbortRequest();                     // Abort button pressed
//
	void onDeviceConnectedSlot();
	void onDeviceDisconnectedSlot();
	void onFilterPositionChangedSlot(int currentPos);
	void onTargetPositionReachedSlot(int targetPos);
	void onFilterMovementAbortedSlot(int currentPos);

protected:
	const QScopedPointer<Ui::FilterCntlPanel> m_ui;

private:
	void updateProfile();
	std::shared_ptr<FilterT> getFilter() const;

	void startAnimation();
	void stopAnimation();

	QMovie * mMovie;

	FocusFinderLogicT & mFfl;
	std::shared_ptr<FilterT> mFilterDevice;

	boost::signals2::connection mDeviceConnectedConnection;
	boost::signals2::connection mDeviceDisconnectedConnection;
	boost::signals2::connection mFilterPositionChangedConnection;
	boost::signals2::connection mTargetPositionReachedConnection;
	boost::signals2::connection mFilterMovementAbortedConnection;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FILTER_CNTL_PANEL_H_*/
