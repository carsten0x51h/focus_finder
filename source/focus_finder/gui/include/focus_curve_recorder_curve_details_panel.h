//
// Created by devnull on 9/8/19.
//

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_CURVE_DETAILS_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_CURVE_DETAILS_PANEL_H_

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
class FocusCurveRecorderCurveDetailsPanelT : public QWidget
{
    Q_OBJECT

public:
  /**
   *  @brief
   *    Constructor
   */
  FocusCurveRecorderCurveDetailsPanelT(QWidget * parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic);

  /**
   *  @brief
   *    Destructor
   */
  virtual ~FocusCurveRecorderCurveDetailsPanelT();

  void reset();
  void setCurveDetails(std::shared_ptr<const FocusCurveT> focusCurve);

protected slots:

protected:
  const QScopedPointer<Ui::FocusCurveRecorderCurveDetailsPanel> m_ui;

private:
  std::shared_ptr<FocusCurveRecorderLogicT> mFocusCurveRecorderLogic;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_CURVE_DETAILS_PANEL_H_*/