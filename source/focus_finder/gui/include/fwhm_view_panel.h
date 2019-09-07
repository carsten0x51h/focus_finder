#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FWHM_VIEW_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FWHM_VIEW_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QPixmap>

#include "../include/fwhm_view_widget.h"

#include "../../common/include/focus_finder_logic.h"
#include "../../common/include/fwhm.h"

namespace Ui {
    class FwhmViewPanel;
}

/**
*  @brief
*/
class FwhmViewPanelT : public QWidget
{
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
FwhmViewPanelT(QWidget * parent, FocusFinderLogicT & ffl);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FwhmViewPanelT();

	void reset();
	void setFwhm(const FwhmT & fwhm);

signals:

protected slots:

protected:
	const QScopedPointer<Ui::FwhmViewPanel> m_ui;

private:
	FocusFinderLogicT & mFfl;
	FwhmViewWidgetT * mFwhmWidget;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FWHM_VIEW_PANEL_H_*/
