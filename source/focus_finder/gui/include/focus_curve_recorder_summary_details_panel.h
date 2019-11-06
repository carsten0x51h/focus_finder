//
// Created by devnull on 9/8/19.
//

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_SUMMARY_DETAILS_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_SUMMARY_DETAILS_PANEL_H_

// std includes

// Qt includes
#include <QWidget>

class FocusCurveRecorderLogicT;

namespace Ui {
    class FocusCurveRecorderSummaryDetailsPanel;
}

/**
*  @brief
*/
class FocusCurveRecorderSummaryDetailsPanelT : public QWidget
{
    Q_OBJECT

public:
  /**
   *  @brief
   *    Constructor
   */
  FocusCurveRecorderSummaryDetailsPanelT(QWidget * parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic);

  /**
   *  @brief
   *    Destructor
   */
  virtual ~FocusCurveRecorderSummaryDetailsPanelT();

  void reset();
  
public slots:
								  
protected slots:

protected:
  const QScopedPointer<Ui::FocusCurveRecorderSummaryDetailsPanel> m_ui;

private:
  std::shared_ptr<FocusCurveRecorderLogicT> mFocusCurveRecorderLogic;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_SUMMARY_DETAILS_PANEL_H_*/
