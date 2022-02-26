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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_STAR_DETAILS_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_STAR_DETAILS_PANEL_H_ SOURCE_FOCUS_FINDER_GUI_INCLUDE_STAR_DETAILS_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <string>

#include "../../common/include/focus_finder_logic.h"
#include "../../common/include/star_details.h"
#include "../../common/include/events/new_frame_event.h"

namespace Ui {
    class StarDetailsPanel;
}

/**
*  @brief
*/
class StarDetailsPanelT : public QWidget {
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
    explicit StarDetailsPanelT(QWidget *parent);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~StarDetailsPanelT();

    void reset();

    void setStarDetails(const StarDetailsT &starDetails);

signals:

protected slots:

protected:
    const QScopedPointer<Ui::StarDetailsPanel> m_ui;

private:
    static std::string floatToString(float f);
    void setStarDetailsFromEvent(const NewFrameEventT & newFrameEvent);
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_STAR_DETAILS_PANEL_H_*/
