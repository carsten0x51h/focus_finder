/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c.schmitt51h@gmail.com>
 *
 *  More info on https://www.lost-infinity.com
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_DIALOG_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_DIALOG_H_ SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_DIALOG_H_

// std includes

// Qt includes
#include <QDialog>
#include <QPixmap>
#include <QHBoxLayout>
#include <QAbstractButton>

#include "anim_menu_button.h"

#include "../../common/include/focus_finder_logic.h"
#include "../../common/include/focus_finder_profile.h"
#include "../../common/include/focus_curve_recorder.h"

class FocusCurveViewPanelT;

class FocusCurveRecorderProgressDetailsPanelT;

class FocusCurveRecorderCurveDetailsPanelT;

class FocusCurveRecorderPointDetailsPanelT;

class FocusCurveRecorderSummaryDetailsPanelT;

namespace Ui {
    class FocusCurveRecorderDialog;
}

enum FocusCurveRecorderDetailViewE {
    SUMMARY, CURVE, POINT, PROGRESS
};

/**
*  @brief
*/
class FocusCurveRecorderDialogT : public QDialog {
Q_OBJECT

public:
    /**
     *  @brief
     *    Constructor
     */
    FocusCurveRecorderDialogT(QWidget *parent, const std::shared_ptr<FocusCurveRecorderLogicT>& focusCurveRecorderLogic);

    /**
     *  @brief
     *    Destructor
     */
    ~FocusCurveRecorderDialogT() override;

    void reset();

    void selectDetailView(FocusCurveRecorderDetailViewE detailView);

signals:

    void focusCurveRecorderStartedSignal();

    void focusCurveRecorderNewRecordSignal(std::shared_ptr<FocusCurveRecordT> focusCurveRecord);

    void focusCurveRecorderRecordSetUpdateSignal(std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet);

    void focusCurveRecorderProgressUpdateSignal(float progress, const QString &msg,
                                                std::shared_ptr<FocusCurveRecordT> focusCurveRecord);

    void focusCurveRecorderRecordSetFinishedSignal(std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet);

    void
    focusCurveRecorderFinishedSignal(std::shared_ptr<const FocusCurveRecordSetContainerT> focusCurveRecordSetContainer);

    void focusCurveRecorderCancelledSignal();


protected slots:

    void onPushButtonPressed();

    //void on_buttonBox_clicked(QAbstractButton *button);
    void onSpinFocusMeasureLimitValueChanged(double value);

    void onSpinNumFocusCurvesToRecordValueChanged(int value);

protected:
    //void closeEvent(QCloseEvent *event) override;

    void accept() override;

    void reject() override;

    void buttonBoxClicked(QAbstractButton *button);

    const QScopedPointer<Ui::FocusCurveRecorderDialog> m_ui;

protected slots:

    // Setting elements
    void onFocusCurveTypeSelectionChanged();

private:
    void applyAction();

    void rejectAction();

    QPushButton *getOkButton();

    QPushButton *getApplyButton();

    void setupButtonBox();

    QHBoxLayout *getMainToolBar();

    //void initFocusMeasureCombobox();

    static void setBtnIcon(QAbstractButton *btn, const std::string &filename);

    [[nodiscard]] bool deviceCheck() const;

    void createMainToolBar();

    void createFocusCurveRecordButton();

    void setFocusCurveRecordButtonState(bool isRunning);


    void onFocusCurveRecorderStarted();

    void onFocusCurveRecorderNewRecord(const std::shared_ptr<FocusCurveRecordT>& focusCurveRecord);

    void onFocusCurveRecorderRecordSetUpdate(const std::shared_ptr<FocusCurveRecordSetT>& focusCurveRecordSet);

    void onFocusCurveRecorderProgressUpdate(float progress, const QString &msg,
                                            std::shared_ptr<FocusCurveRecordT> focusCurveRecord);

    void onFocusCurveRecorderCancelled();

    void onFocusCurveRecorderRecordSetFinished(const std::shared_ptr<FocusCurveRecordSetT>& focusCurveRecordSet);

    void
    onFocusCurveRecorderFinished(const std::shared_ptr<const FocusCurveRecordSetContainerT>& focusCurveRecordSetContainer);

    void onFocusCurveRecordPressed(bool isChecked);


    FocusFinderProfileT mActiveProfileTmp;

    AnimMenuButtonT *mFocusCurveRecordButton{};

    // TODO: Is it ok that all those instances are local to this GUI window?
    std::shared_ptr<TaskExecutorT<FocusCurveRecorderT> > mRecorderExec;

    std::shared_ptr<FocusCurveRecorderLogicT> mFocusCurveRecorderLogic;
    FocusCurveViewPanelT *mFocusCurveViewPanel;
    FocusCurveRecorderProgressDetailsPanelT *mFocusCurveRecorderProgressDetailsPanel;
    FocusCurveRecorderCurveDetailsPanelT *mFocusCurveRecorderCurveDetailsPanel;
    FocusCurveRecorderPointDetailsPanelT *mFocusCurveRecorderPointDetailsPanel;
    FocusCurveRecorderSummaryDetailsPanelT *mFocusCurveRecorderSummaryDetailsPanel;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_DIALOG_H_*/
