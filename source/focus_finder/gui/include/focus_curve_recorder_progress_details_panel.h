//
// Created by devnull on 9/8/19.
//

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_PROGRESS_DETAILS_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_PROGRESS_DETAILS_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QPixmap>
#include <QHBoxLayout>
#include <QProgressBar>

class FocusCurveRecorderLogicT;

namespace Ui {
    class FocusCurveRecorderProgressDetailsPanel;
}

/**
*  @brief
*/
class FocusCurveRecorderProgressDetailsPanelT : public QWidget
{
    Q_OBJECT

public:
  /**
   *  @brief
   *    Constructor
   */
  FocusCurveRecorderProgressDetailsPanelT(QWidget * parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic);

  /**
   *  @brief
   *    Destructor
   */
  virtual ~FocusCurveRecorderProgressDetailsPanelT();

  void reset();

  void setIteration(size_t currentIteration, size_t numTotalIterations);
  void setCurrentIterationProgress(int progressPerc);
  void setCurrentIterationProgressText(const QString & currentProgressText);
  void setTotalProgress(int progressPerc);
  void setTotalProgressText(const QString & totalProgressText);

  void startAnimation();
  void stopAnimation();
  
public slots:
  // member function that catches the frameChanged signal of the QMovie
  void setButtonIcon(int frame);
								  
protected slots:

protected:
  const QScopedPointer<Ui::FocusCurveRecorderProgressDetailsPanel> m_ui;

private:
  void setProgress(QProgressBar * progressBar, int progressPerc);

  std::shared_ptr<FocusCurveRecorderLogicT> mFocusCurveRecorderLogic;
   QMovie * mMovie;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_PROGRESS_DETAILS_PANEL_H_*/
