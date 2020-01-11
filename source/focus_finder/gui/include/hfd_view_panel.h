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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_HFD_VIEW_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_HFD_VIEW_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QPixmap>

#include "../include/hfd_view_widget.h"

#include "../../common/include/hfd.h"

namespace Ui {
    class HfdViewPanel;
}

/**
*  @brief
*/
class HfdViewPanelT : public QWidget
{
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
	HfdViewPanelT(QWidget * parent);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~HfdViewPanelT();

	void reset();
	void setHfd(const HfdT & hfd);

signals:

protected slots:

protected:
	const QScopedPointer<Ui::HfdViewPanel> m_ui;

private:
	HfdViewWidgetT * mHfdWidget;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_HFD_VIEW_PANEL_H_*/
