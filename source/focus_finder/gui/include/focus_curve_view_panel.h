#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QPixmap>

#include "../include/focus_curve_view_widget.h"

#include "../../common/include/focus_curve.h"
#include "../../common/include/focus_curve_recorder_logic.h"

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
  FocusCurveViewPanelT(QWidget * parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FocusCurveViewPanelT();

	void reset();
  //void setFocusCurve(std::shared_ptr<const FocusCurveT> focusCurve);
  void update();

  // FocusMeasureTypeT::TypeE getFocusMeasureType() const;
  // void setFocusMeasureType(FocusMeasureTypeT::TypeE focusMeasureType);


signals:

protected slots:

protected:
	const QScopedPointer<Ui::FocusCurveViewPanel> m_ui;

private:
  std::shared_ptr<FocusCurveRecorderLogicT> mFocusCurveRecorderLogic;
  FocusCurveViewWidgetT * mFocusCurveWidget;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_PANEL_H_*/


