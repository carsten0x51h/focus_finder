//
// Created by devnull on 9/8/19.
//

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_POINT_DETAILS_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_POINT_DETAILS_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QPixmap>
#include <QHBoxLayout>

class FocusCurveRecorderLogicT;

namespace Ui {
    class FocusCurveRecorderPointDetailsPanel;
}

/**
*  @brief
*/
class FocusCurveRecorderPointDetailsPanelT : public QWidget
{
    Q_OBJECT

public:
  /**
   *  @brief
   *    Constructor
   */
  FocusCurveRecorderPointDetailsPanelT(QWidget * parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic);

  /**
   *  @brief
   *    Destructor
   */
  virtual ~FocusCurveRecorderPointDetailsPanelT();

  void reset();

protected slots:

protected:
  const QScopedPointer<Ui::FocusCurveRecorderPointDetailsPanel> m_ui;

private:
  std::shared_ptr<FocusCurveRecorderLogicT> mFocusCurveRecorderLogic;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_POINT_DETAILS_PANEL_H_*/
