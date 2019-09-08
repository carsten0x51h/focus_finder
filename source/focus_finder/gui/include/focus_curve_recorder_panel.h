//
// Created by devnull on 9/8/19.
//

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QPixmap>

#include "../include/focus_curve_recorder_panel.h"
#include "../../common/include/focus_finder_logic.h"

namespace Ui {
    class FocusCurveRecorderPanel;
}

/**
*  @brief
*/
class FocusCurveRecorderPanelT : public QWidget
{
    Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
    FocusCurveRecorderPanelT(QWidget * parent, FocusFinderLogicT & ffl);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FocusCurveRecorderPanelT();

    void reset();

    signals:

protected slots:

protected:
    const QScopedPointer<Ui::FocusCurveRecorderPanel> m_ui;

private:
    FocusFinderLogicT & mFfl;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_PANEL_H_*/
