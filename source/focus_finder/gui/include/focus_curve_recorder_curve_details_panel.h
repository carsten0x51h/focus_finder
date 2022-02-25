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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_CURVE_DETAILS_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_CURVE_DETAILS_PANEL_H_ SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_CURVE_DETAILS_PANEL_H_

// std includes
#include <memory>

// Qt includes
#include <QWidget>
#include <QPixmap>
#include <QHBoxLayout>

#include "../../common/include/focus_curve.h"

class FocusCurveRecorderLogicT;

namespace Ui {
    class FocusCurveRecorderCurveDetailsPanel;
}

/**
*  @brief
*/
class FocusCurveRecorderCurveDetailsPanelT : public QWidget {
Q_OBJECT

public:
    /**
     *  @brief
     *    Constructor
     */
    FocusCurveRecorderCurveDetailsPanelT(QWidget *parent,
                                         std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic);

    /**
     *  @brief
     *    Destructor
     */
    ~FocusCurveRecorderCurveDetailsPanelT() override;

    void reset();

    void setCurveDetails(const std::shared_ptr<const FocusCurveT>& focusCurve);

protected slots:

protected:
    const QScopedPointer<Ui::FocusCurveRecorderCurveDetailsPanel> m_ui;

private:
    std::shared_ptr<FocusCurveRecorderLogicT> mFocusCurveRecorderLogic;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_CURVE_DETAILS_PANEL_H_*/
