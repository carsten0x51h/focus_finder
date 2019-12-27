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
#include "../../common/include/focus_finder_profile.h"
#include "../../common/include/focus_curve_recorder.h"

class FocusCurveViewPanelT;
class FocusCurveRecorderProgressDetailsPanelT;
class FocusCurveRecorderCurveDetailsPanelT;
class FocusCurveRecorderPointDetailsPanelT;
class FocusCurveRecorderSummaryDetailsPanelT;

namespace Ui {
    class FocusCurveRecorderPanel;
}

enum FocusCurveRecorderDetailViewE { SUMMARY, CURVE, POINT, PROGRESS };

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
  FocusCurveRecorderPanelT(QWidget * parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic);

  /**
   *  @brief
   *    Destructor
   */
  virtual ~FocusCurveRecorderPanelT();

  void reset();
  void selectDetailView(FocusCurveRecorderDetailViewE detailView);
  
signals:
  void focusCurveRecorderStartedSignal();
  void focusCurveRecorderNewRecordSignal(std::shared_ptr<FocusCurveRecordT> focusCurveRecord);
  void focusCurveRecorderRecordSetUpdateSignal(std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet);
  void focusCurveRecorderProgressUpdateSignal(float progress, const QString & msg, std::shared_ptr<FocusCurveRecordT> focusCurveRecord);
  void focusCurveRecorderRecordSetFinishedSignal(std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet);
  void focusCurveRecorderFinishedSignal(std::shared_ptr<const FocusCurveRecordSetContainerT> focusCurveRecordSetContainer);
  void focusCurveRecorderCancelledSignal();

					  
protected slots:
  void onPushButtonPressed();
  void on_buttonBox_clicked(QAbstractButton *button);

  
protected:
    const QScopedPointer<Ui::FocusCurveRecorderPanel> m_ui;
protected slots:
  // Setting elements
  void onFocusCurveTypeSelectionChanged();

private:
  FocusFinderProfileT mActiveProfileTmp;

  QHBoxLayout * getMainToolBar();

  void addFocusCurveType(FittingCurveTypeT::TypeE focusCurveType);
  //void initFocusMeasureCombobox();
  void initFocusCurveTypeCombobox();
  
  void setBtnIcon(QAbstractButton * btn, const std::string & filename);
  bool deviceCheck() const;

  void createMainToolBar();
  void createFocusCurveRecordButton();
  void setFocusCurveRecordButtonState(bool isRunning);


  void onFocusCurveRecorderStarted();
  void onFocusCurveRecorderNewRecord(std::shared_ptr<FocusCurveRecordT> focusCurveRecord);
  void onFocusCurveRecorderRecordSetUpdate(std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet);
  void onFocusCurveRecorderProgressUpdate(float progress, const QString & msg, std::shared_ptr<FocusCurveRecordT> focusCurveRecord);
  void onFocusCurveRecorderCancelled();
  void onFocusCurveRecorderRecordSetFinished(std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet);
  void onFocusCurveRecorderFinished(std::shared_ptr<const FocusCurveRecordSetContainerT> focusCurveRecordSetContainer);
  void onFocusCurveRecordPressed(bool isChecked);

  
  
  AnimMenuButtonT * mFocusCurveRecordButton;

  // TODO: Is it ok that all those instances are local to this GUI window?
  std::shared_ptr<TaskExecutorT<FocusCurveRecorderT> > mRecorderExec;
  
  std::shared_ptr<FocusCurveRecorderLogicT> mFocusCurveRecorderLogic;
  FocusCurveViewPanelT * mFocusCurveViewPanel;
  FocusCurveRecorderProgressDetailsPanelT * mFocusCurveRecorderProgressDetailsPanel;
  FocusCurveRecorderCurveDetailsPanelT * mFocusCurveRecorderCurveDetailsPanel; 
  FocusCurveRecorderPointDetailsPanelT * mFocusCurveRecorderPointDetailsPanel;
  FocusCurveRecorderSummaryDetailsPanelT * mFocusCurveRecorderSummaryDetailsPanel;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_PANEL_H_*/
