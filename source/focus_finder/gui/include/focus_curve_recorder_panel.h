//
// Created by devnull on 9/8/19.
//

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QPixmap>
#include <QHBoxLayout>
#include <QAbstractButton>

#include "anim_menu_button.h"

#include "../include/focus_curve_recorder_panel.h"
#include "../../common/include/focus_finder_logic.h"
#include "../../common/include/focus_curve_recorder.h"

class FocusCurveViewPanelT;

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
  void focusCurveRecorderStartedSignal();
  void focusCurveRecorderProgressUpdateSignal(float progress, const QString & msg, std::shared_ptr<FocusCurveRecordT> focusCurveRecord);
  void focusCurveRecorderFinishedSignal(bool lastCurve);
  void focusCurveRecorderCancelledSignal();

protected slots:

protected:
    const QScopedPointer<Ui::FocusCurveRecorderPanel> m_ui;

private:
  QHBoxLayout * getMainToolBar();
  
  void setBtnIcon(QAbstractButton * btn, const std::string & filename);
  bool deviceCheck() const;

  void createMainToolBar();
  void createFocusCurveRecordButton();
  void setFocusCurveRecordButtonState(bool isRunning);


  void onFocusCurveRecorderStarted();
  void onFocusCurveRecorderProgressUpdate(float progress, const QString & msg, std::shared_ptr<FocusCurveRecordT> focusCurveRecord);
  void onFocusCurveRecorderCancelled();
  void onFocusCurveRecorderFinished(bool isLastCurve);

  void onFocusCurveRecordPressed(bool isChecked);

  AnimMenuButtonT * mFocusCurveRecordButton;

  // TODO: Is it ok that all those instances are local to this GUI window?
  std::shared_ptr<TaskExecutorT<FocusCurveRecorderT> > mRecorderExec;
  
  FocusFinderLogicT & mFfl;

  FocusCurveViewPanelT * mFocusCurveViewPanel;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_PANEL_H_*/
