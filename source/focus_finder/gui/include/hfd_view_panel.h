#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_HFD_VIEW_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_HFD_VIEW_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QPixmap>

#include "../include/hfd_view_widget.h"

#include "../../common/include/focus_finder_logic.h"
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
	HfdViewPanelT(QWidget * parent, FocusFinderLogicT & ffl);

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
	FocusFinderLogicT & mFfl;
	HfdViewWidgetT * mHfdWidget;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_HFD_VIEW_PANEL_H_*/
