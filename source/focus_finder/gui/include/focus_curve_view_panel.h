#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QPixmap>

#include "../include/focus_curve_view_widget.h"

#include "../../common/include/focus_finder_logic.h"
#include "../../common/include/focus_curve.h"

namespace Ui {
    class FocusCurveViewPanel;
}

/**
*  @brief
*/
class FocusCurveViewPanelT : public QWidget
{
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
FocusCurveViewPanelT(QWidget * parent, FocusFinderLogicT & ffl);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FocusCurveViewPanelT();

	void reset();
	void setFocusCurve(const FocusCurveT & focusCurve);

signals:

protected slots:

protected:
	const QScopedPointer<Ui::FocusCurveViewPanel> m_ui;

private:
	FocusFinderLogicT & mFfl;
	FocusCurveViewWidgetT * mFocusCurveWidget;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_PANEL_H_*/


