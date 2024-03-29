/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c [at] lost-infinity.com>
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

#include <memory>

#include <QWidget>
#include <QHBoxLayout>
#include <QAbstractButton>
#include <QDoubleSpinBox>
#include <utility>

#include "include/focus_curve_recorder_dialog.h"
#include "include/focus_curve_view_panel.h"
#include "include/focus_curve_recorder_progress_details_panel.h"
#include "include/focus_curve_recorder_curve_details_panel.h"
#include "include/focus_curve_recorder_point_details_panel.h"
#include "include/focus_curve_recorder_summary_details_panel.h"

#include "../common/include/profile_manager.h"

//#include "../common/include/task_executor.h"
//#include "../common/include/focus_curve_recorder.h"
//#include "../common/include/focus_curve_record.h"
//#include "../common/include/focus_curve_recorder_type.h"
//#include "../common/include/focus_curve_recorder_factory.h"
//#include "../common/include/focus_curve_recorder_logic.h"
//#include "../common/include/focus_curve_record_set.h"
//#include "../common/include/focus_measure_type.h"
//#include "../common/include/fitting_curve_type.h"

#include "ui_focus_curve_recorder_dialog.h"

Q_DECLARE_METATYPE(FocusMeasureTypeT::TypeE)

Q_DECLARE_METATYPE(FittingCurveTypeT::TypeE)

Q_DECLARE_METATYPE(FocusCurveTypeT::TypeE)

Q_DECLARE_METATYPE(std::shared_ptr<FocusCurveRecordSetT>)


// TODO: Actually this is duplicated code - see main_window.cpp...
void FocusCurveRecorderDialogT::setBtnIcon(QAbstractButton *btn,
                                           const std::string &filename) {
    QIcon icon;

    icon.addFile(QString::fromStdString(filename), QSize(), QIcon::Normal,
                 QIcon::Off);

    btn->setIcon(icon);
    btn->setMinimumSize(QSize(48, 48));
    btn->setIconSize(QSize(48, 48));
}

FocusCurveRecorderDialogT::FocusCurveRecorderDialogT(QWidget *parent,
                                                     const std::shared_ptr<FocusCurveRecorderLogicT>& focusCurveRecorderLogic)
        : QDialog(parent),
          m_ui(new Ui::FocusCurveRecorderDialog),
          mFocusCurveRecorderLogic(focusCurveRecorderLogic) {
    // Setup UI
    m_ui->setupUi(this);

    setupButtonBox();

    createMainToolBar();
//    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    sizePolicy.setHorizontalStretch(100);
//    sizePolicy.setVerticalStretch(100);
//
//    mFocusCurveWidget = new FocusCurveViewWidgetT(m_ui->widget);
//    mFocusCurveWidget->setSizePolicy(sizePolicy);
//    m_ui->layFocusCurveViewWidget->addWidget(mFocusCurveWidget, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);

    // TODO: Move to reset?
    mRecorderExec = std::make_shared<TaskExecutorT<FocusCurveRecorderT> >();

    // Add the focus curve viewer panel
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(100);
    sizePolicy.setVerticalStretch(100);

    mFocusCurveViewPanel = new FocusCurveViewPanelT(m_ui->widget, mFocusCurveRecorderLogic);
    mFocusCurveViewPanel->setSizePolicy(sizePolicy);
    m_ui->layFocusCurveViewPanel->addWidget(mFocusCurveViewPanel, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);


    // Configure setting elements
    /////////////////////////////

    connect(m_ui->cbxFocusCurveType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FocusCurveRecorderDialogT::onFocusCurveTypeSelectionChanged);


    // Add progress details panel
    mFocusCurveRecorderSummaryDetailsPanel = new FocusCurveRecorderSummaryDetailsPanelT(m_ui->wgtDetailsWidget,
                                                                                        focusCurveRecorderLogic);
    m_ui->layDetailsWidget->addWidget(mFocusCurveRecorderSummaryDetailsPanel, 0/*row*/, 0/*col*/, 1/*rowspan*/,
                                      1/*colspan*/);

    mFocusCurveRecorderProgressDetailsPanel = new FocusCurveRecorderProgressDetailsPanelT(m_ui->wgtDetailsWidget,
                                                                                          focusCurveRecorderLogic);
    m_ui->layDetailsWidget->addWidget(mFocusCurveRecorderProgressDetailsPanel, 0/*row*/, 0/*col*/, 1/*rowspan*/,
                                      1/*colspan*/);
    mFocusCurveRecorderProgressDetailsPanel->setVisible(false);

    mFocusCurveRecorderCurveDetailsPanel = new FocusCurveRecorderCurveDetailsPanelT(m_ui->wgtDetailsWidget,
                                                                                    focusCurveRecorderLogic);
    m_ui->layDetailsWidget->addWidget(mFocusCurveRecorderCurveDetailsPanel, 0/*row*/, 0/*col*/, 1/*rowspan*/,
                                      1/*colspan*/);
    mFocusCurveRecorderCurveDetailsPanel->setVisible(false);

    mFocusCurveRecorderPointDetailsPanel = new FocusCurveRecorderPointDetailsPanelT(m_ui->wgtDetailsWidget,
                                                                                    focusCurveRecorderLogic);
    m_ui->layDetailsWidget->addWidget(mFocusCurveRecorderPointDetailsPanel, 0/*row*/, 0/*col*/, 1/*rowspan*/,
                                      1/*colspan*/);
    mFocusCurveRecorderPointDetailsPanel->setVisible(false);

    //mFocusCurveRecorderProgressDetailsPanel->setParent(m_ui->wgtDetailsWidget);
    //mFocusCurveRecorderCurveDetailsPanel->setParent(m_ui->wgtDetailsWidget);
    //mFocusCurveRecorderProgressDetailsPanel->startAnimation();

    // Take a copy from the active profile.
    // TODO: If there is no active profile, the user should not be able to come here...
    //       However, in that case, all should be disabled. User can just press Ok & Cancel.
    auto activeProfileOpt = mFocusCurveRecorderLogic->getActiveProfile();

    mActiveProfileTmp = activeProfileOpt.value();


    // Add change listeners
    connect(m_ui->spinFocusMeasureLimit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &FocusCurveRecorderDialogT::onSpinFocusMeasureLimitValueChanged);

    connect(m_ui->spinNumFocusCurvesToRecord, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &FocusCurveRecorderDialogT::onSpinNumFocusCurvesToRecordValueChanged);



    // HACK - REMOVE LATER!
    connect(m_ui->pushButton, &QPushButton::pressed, this, &FocusCurveRecorderDialogT::onPushButtonPressed);


    reset();
}

void FocusCurveRecorderDialogT::onSpinFocusMeasureLimitValueChanged(double value) {
    LOG(debug) << "FocusCurveRecorderDialogT::onSpinFocusMeasureLimitValueChanged... value=" << value << std::endl;

    mActiveProfileTmp.setFocusMeasureLimit((float) value);
}

void FocusCurveRecorderDialogT::onSpinNumFocusCurvesToRecordValueChanged(int value) {
    LOG(debug) << "FocusCurveRecorderDialogT::onSpinNumFocusCurvesToRecordValueChanged... value=" << value << std::endl;

    mActiveProfileTmp.setNumberCurvesToRecord(value);
}

void FocusCurveRecorderDialogT::selectDetailView(FocusCurveRecorderDetailViewE detailView) {
    LOG(debug) << "FocusCurveRecorderDialogT::selectDetailView... detailView=" << detailView << std::endl;

    switch (detailView) {
        case SUMMARY:
            mFocusCurveRecorderSummaryDetailsPanel->setVisible(true);
            mFocusCurveRecorderCurveDetailsPanel->setVisible(false);
            mFocusCurveRecorderPointDetailsPanel->setVisible(false);
            mFocusCurveRecorderProgressDetailsPanel->setVisible(false);
            break;
        case CURVE:
            mFocusCurveRecorderSummaryDetailsPanel->setVisible(false);
            mFocusCurveRecorderCurveDetailsPanel->setVisible(true);
            mFocusCurveRecorderPointDetailsPanel->setVisible(false);
            mFocusCurveRecorderProgressDetailsPanel->setVisible(false);
            break;
        case POINT:
            mFocusCurveRecorderSummaryDetailsPanel->setVisible(false);
            mFocusCurveRecorderCurveDetailsPanel->setVisible(false);
            mFocusCurveRecorderPointDetailsPanel->setVisible(true);
            mFocusCurveRecorderProgressDetailsPanel->setVisible(false);
            break;
        case PROGRESS:
            mFocusCurveRecorderSummaryDetailsPanel->setVisible(false);
            mFocusCurveRecorderCurveDetailsPanel->setVisible(false);
            mFocusCurveRecorderPointDetailsPanel->setVisible(false);
            mFocusCurveRecorderProgressDetailsPanel->setVisible(true);
            break;
        default:
            LOG(error) << "Invalid detailView " << detailView << ". Not changing the view!" << std::endl;
            break;
    }
}

void FocusCurveRecorderDialogT::onPushButtonPressed() {
    static int idx = 0;

    ++idx;
    idx = idx % 4;

    auto detailView = static_cast<FocusCurveRecorderDetailViewE>(idx);
    selectDetailView(detailView);
}

void FocusCurveRecorderDialogT::onFocusCurveTypeSelectionChanged() {
    QVariant cbxData = m_ui->cbxFocusCurveType->currentData();
    auto focusCurveType = cbxData.value<FocusCurveTypeT::TypeE>();

    LOG(debug) << "FocusCurveRecorderDialogT::onFocusCurveTypeSelectionChanged...focusCurveType="
               << FocusCurveTypeT::asStr(focusCurveType) << std::endl;

    CurveFitParmsT curveFitParms = mActiveProfileTmp.getFocusCurveMatchingParms();

    curveFitParms.setFittingCurveType(FocusCurveTypeT::toFittingCurve(focusCurveType));

    mActiveProfileTmp.setFocusCurveMatchingParms(curveFitParms);
}


FocusCurveRecorderDialogT::~FocusCurveRecorderDialogT() {
    delete mFocusCurveRecordButton; // TODO: Ok?
}

void FocusCurveRecorderDialogT::reset() {

    // Set focus measure limit
    m_ui->spinFocusMeasureLimit->setValue(mActiveProfileTmp.getFocusMeasureLimit());

    // Set num focus curves to record
    m_ui->spinNumFocusCurvesToRecord->setValue(mActiveProfileTmp.getNumberCurvesToRecord());

    // Fill the focus curve shape...
    CurveFitParmsT curveFitParms = mActiveProfileTmp.getFocusCurveMatchingParms();
    FocusCurveTypeT::TypeE focusCurveTypeToSelect = FocusCurveTypeT::fromFittingCurve(
            curveFitParms.getFittingCurveType());

    m_ui->cbxFocusCurveType->blockSignals(true);
    m_ui->cbxFocusCurveType->clear();

    for (size_t idx = 0; idx < FocusCurveTypeT::_Count; ++idx) {
        auto focusCurveType = static_cast<FocusCurveTypeT::TypeE>(idx);

        QVariant data;
        data.setValue(focusCurveType);

        m_ui->cbxFocusCurveType->addItem(QString::fromStdString(FocusCurveTypeT::asStr(focusCurveType)), data);
    }

    m_ui->cbxFocusCurveType->blockSignals(false);

    // Select the entry from the profile...
    int idxToSelect = m_ui->cbxFocusCurveType->findText(
            QString::fromStdString(FocusCurveTypeT::asStr(focusCurveTypeToSelect)));

    m_ui->cbxFocusCurveType->setCurrentIndex(idxToSelect);
}


QPushButton *FocusCurveRecorderDialogT::getOkButton() {
    return m_ui->buttonBox->button(QDialogButtonBox::Ok);
}

QPushButton *FocusCurveRecorderDialogT::getApplyButton() {
    return m_ui->buttonBox->button(QDialogButtonBox::Apply);
}

void FocusCurveRecorderDialogT::setupButtonBox() {
    getOkButton()->setDefault(true);
    getOkButton()->setShortcut(Qt::CTRL | Qt::Key_Return);

    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &FocusCurveRecorderDialogT::accept);
    connect(m_ui->buttonBox, &QDialogButtonBox::rejected, this, &FocusCurveRecorderDialogT::reject);
    connect(m_ui->buttonBox, &QDialogButtonBox::clicked, this, &FocusCurveRecorderDialogT::buttonBoxClicked);
}

void FocusCurveRecorderDialogT::applyAction() {
    LOG(debug) << "FocusCurveRecorderDialogT::applyAction..." << std::endl;

    auto profileManager = mFocusCurveRecorderLogic->getProfileManager();

    profileManager->updateProfile(profileManager->getActiveProfileDirectoryName() /*profileDirectoryName*/,
                                  mActiveProfileTmp);
}

void FocusCurveRecorderDialogT::rejectAction() {
    if (mRecorderExec->isRunning()) {
        mRecorderExec->cancel();
    }
}

void FocusCurveRecorderDialogT::accept() {
    LOG(debug) << "FocusCurveRecorderDialogT::accept..." << std::endl;
    applyAction();
    QDialog::accept();
}

void FocusCurveRecorderDialogT::reject() {
    LOG(debug) << "FocusCurveRecorderDialogT::reject..." << std::endl;
    rejectAction();
    QDialog::reject();
}

void FocusCurveRecorderDialogT::buttonBoxClicked(QAbstractButton *button) {
    LOG(debug) << "FocusCurveRecorderDialogT::buttonBoxClicked..." << std::endl;
    QDialogButtonBox::ButtonRole buttonRole = m_ui->buttonBox->buttonRole(button);

    if (buttonRole == QDialogButtonBox::ApplyRole) {
        LOG(debug) << "FocusCurveRecorderDialogT::buttonBoxClicked... APPLY clicked..." << std::endl;
        applyAction();
    }
}

void FocusCurveRecorderDialogT::onFocusCurveRecordPressed(bool isChecked) {
    LOG(debug) << "FocusCurveRecorderDialogT::onFocusCurveRecordPressed... mFocusCurveRecordButton->isChecked(): "
               << isChecked << std::endl;

    if (isChecked) {
        // HACK / TODO: For now we create always a new instance...
        // TODO: Do not create here?! -> Factory?
        mFocusCurveRecorderLogic->resetFocusCurveRecorder(FocusCurveRecorderTypeT::DEFAULT, mActiveProfileTmp);

        auto focusCurveRecorder = mFocusCurveRecorderLogic->getFocusCurveRecorder();

        try {
            focusCurveRecorder->getFocusController()->devicesAvailabilityCheck();
        } catch (FocusControllerFailedExceptionT &exc) {
            // TODO: Log error / warning...
            return;
        }

        // Read properties from UI
        // Reading
        //int idx = cbxFocusMeasure->currentIndex()
        //m_ui->cbxFocusMeasure->itemData(cbxFocusMeasure->currentIndex());
        //m_ui->spinFocusMeasureLimit;
        //m_ui->spinFocuserStepSize;

        // Matching
        //m_ui->cbxFocusCurveType;
        //m_ui->spinEpsRel;
        //m_ui->spinEpsAbs;
        //m_ui->spinMaxIterations;
        //m_ui->spinOutlierBoundaryFactor;
        //m_ui->spinMaxAcceptedOutliers;


        // TODO: Check that ffExec it is not yet running?

        // TODO: Check that every device is connected?

        // TODO: Always create a new focus finder instance? Otherwise.. where to store?
        //       -> depends on selection... -> FocusFinder profile...

        // TODO: Get value from UI and set it here...

        // TODO: focusFinderProfile....


        //mFocusCurveRecorderLogic->setFocusMeasureType(FocusMeasureTypeT::HFD);


        // Register FocusCurveRecorder UI listeners
        // FocusCurveRecorder started
        focusCurveRecorder->registerFocusCurveRecorderStartedListener([&]() {
            emit focusCurveRecorderStartedSignal();
        });

        // Register listener to get notified when there is a new record available
        qRegisterMetaType<std::shared_ptr<FocusCurveRecordT> >("FocusCurveRecordPtrT");

        focusCurveRecorder->registerFocusCurveRecorderNewRecordListener(
                [&](std::shared_ptr<FocusCurveRecordT> record) {
                    emit focusCurveRecorderNewRecordSignal(std::move(record));
                });

        // Register listener to get notified when the focus curve record set was updated (new data point for curve recorded)
        qRegisterMetaType<std::shared_ptr<FocusCurveRecordSetT> >("FocusCurveRecordSetPtrT");

        focusCurveRecorder->registerFocusCurveRecorderRecordSetUpdateListener(
                [&](std::shared_ptr<FocusCurveRecordSetT> recordSet) {
                    emit focusCurveRecorderRecordSetUpdateSignal(std::move(recordSet));
                });


        // Register progress listener
        focusCurveRecorder->registerFocusCurveRecorderProgressUpdateListener(
                [&](float progress, const std::string &msg, std::shared_ptr<FocusCurveRecordT> record) {
                    emit focusCurveRecorderProgressUpdateSignal(progress, QString::fromStdString(msg), std::move(record));
                });


        // FocusCurveRecorder new focus curve record set finished
        focusCurveRecorder->registerFocusCurveRecorderRecordSetFinishedListener(
                [&](std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet) {
                    emit focusCurveRecorderRecordSetFinishedSignal(std::move(focusCurveRecordSet));
                });

        // FocusCurveRecorder finished
        qRegisterMetaType<std::shared_ptr<const FocusCurveRecordSetContainerT> >("FocusCurveRecordSetContainerPtrT");

        focusCurveRecorder->registerFocusCurveRecorderFinishedListener(
                [&](std::shared_ptr<const FocusCurveRecordSetContainerT> focusCurveRecordSetContainer) {
                    emit focusCurveRecorderFinishedSignal(std::move(focusCurveRecordSetContainer));
                });

        // FocusCurveRecorder cancel
        focusCurveRecorder->registerFocusCurveRecorderCancelledListener([&]() {
            emit focusCurveRecorderCancelledSignal();
        });

        mRecorderExec->setTask(focusCurveRecorder);

        LOG(debug)
            << "Starting focus curve recorder '" << focusCurveRecorder->getName() << "'..." << std::endl;


        selectDetailView(PROGRESS);


        mRecorderExec->start();
    } else {
        LOG(info)
            << "Cancelling running focus curve recorder..." << std::endl;
        mRecorderExec->cancel();

        mFocusCurveRecordButton->setEnabled(false);

        LOG(info)
            << "Running focus curve recorder cancelled." << std::endl;
    }

    //updateFocusFinderMainMenuBar();

    //-----------------------------------------------------------------





    // if (!isChecked) {
    //   LOG(debug)
    //     << "Button already pressed.... -> cancel curve recording."
    // 				<< std::endl;

    //   mFocusCurveRecordButton->setEnabled(false);

    //   // TODO...
    //   //mFocusCurveRecorder->cancelRecording();

    // } else {
    //   LOG(debug)
    //     << "Start curve recording..." << std::endl;

    //   setFocusCurveRecordButtonState(true);

    //   // TODO...
    //   // mFocusCurveRecorder->startRecording();
    // }

    // // TODO...
    // //updateFocusFinderMainMenuBar();
}

// void FocusCurveRecorderDialogT::setFocusCurveRecordButtonState(bool isRunning) {
// 	LOG(debug)
// 	<< "FocusCurveRecorderDialogT::setFocusCurveRecordButtonState...isRunning=" << isRunning
// 			<< std::endl;

// 	if (isRunning) {
// 		mFocusCurveRecordButton->startAnimation();
// 		mFocusCurveRecordButton->setEnabled(true);
// 		mFocusCurveRecordButton->setChecked(true);
// 	} else {
// 		mFocusCurveRecordButton->stopAnimation();
// 		mFocusCurveRecordButton->setEnabled(true);
// 		mFocusCurveRecordButton->setChecked(false);
// 		setBtnIcon(mFocusCurveRecordButton, ":/res/record_focus_curve_64x64.png");
// 	}
// }
void FocusCurveRecorderDialogT::onFocusCurveRecorderRecordSetFinished(
        const std::shared_ptr<FocusCurveRecordSetT>& focusCurveRecordSet) {
    LOG(debug)
        << "FocusCurveRecorderDialogT::onFocusCurveRecorderRecordSetFinished..." << std::endl;


    // // TODO: Remove / replace by real values...
    // const float EPS_REL = 1e-2; // TODO: Do not hardcode
    // const float EPS_ABS = 1e-2; // TODO: Do not hardcode
    // const size_t MAX_NUM_ITER = 10000; // TODO: Do not hardcode?
    // const bool ENABLE_OUTLIER_DETECTION = true;  // TODO: Do not hardcode?
    // const float OUTLIER_BOUNDARY_FACTOR = 1.5F; // TODO: Do not hardcode?
    // const float MAX_ACCEPTED_OUTLIERS_PERC = 20.0F; // TODO: Do not hardcode?
    // const FocusCurveTypeT::TypeE focusCurveType = FocusCurveTypeT::HYPERBOLIC;


    // // Match the curve...
    // CurveFitParmsT curveFitParms(
    // 			     FocusCurveTypeT::toFittingCurve(focusCurveType),
    // 			     EPS_REL, /*epsrel*/
    // 			     EPS_ABS, /*epsabs*/
    // 			     MAX_NUM_ITER, /*maxnumiter*/
    // 			     ENABLE_OUTLIER_DETECTION,
    // 			     OUTLIER_BOUNDARY_FACTOR, /*outlier boundary factor*/
    // 			     MAX_ACCEPTED_OUTLIERS_PERC /* max. accepted outliers perc. */
    // 			     );




    // Add the recorded record set to the tmp profile
    // TODO / NOTE: This should not happen here in the GUI! -> Move this to the FocusCurveRecorderLogic! This one will need its own listener!...
    auto focusFinderCalibration = mActiveProfileTmp.getFocusFinderCalibration(); // std::shared_ptr<FocusFinderCalibrationT>
    FocusCurveRecordSetContainerT &focusCurveRecordSetContainer = focusFinderCalibration->getCalibrationData();
    focusCurveRecordSetContainer.push_back(focusCurveRecordSet);


    CurveFitParmsT curveFitParms = mActiveProfileTmp.getFocusCurveMatchingParms();

    try {
        auto focusCurve = std::make_shared<FocusCurveT>(focusCurveRecordSet, curveFitParms);

        // Print the calculated curve parms
        LOG(info) << "Calculated focus curve parameters: " << std::endl
                  << *focusCurve << std::endl;

        // HACK TO SEE THE CURVE!
        mFocusCurveViewPanel->drawCurveHack(focusCurve);
        mFocusCurveRecorderCurveDetailsPanel->setCurveDetails(focusCurve);

        // HACK TO SEE THE DELTA
        float relFocusPosBoundary = focusCurve->getRelativeFocusPosBoundary();
        LOG(debug) << "relFocusPosBoundary: " << relFocusPosBoundary << std::endl;
    } catch (CurveFitExceptionT &exc) {
        LOG(error) << "Focus curve fitting failed! Reason: " << exc.what() << std::endl;
        // TODO: ReportingT....
    }

    //TODO: What happens to focusCurve?? -> needs to be drawn......

    // TODO: Currently we do not cache the calculatef FocusCurve.
    //       We will find out if this will be required....
    //       If it needs to be stored, we store it on the
    //       FocusCurveRecoderLogic class.
    // TODO:  Store focusCurve in RecorderLogic? Connection to RecordSet? Triggered by this listener? Guess Logic class should have own listener!.. and then calculate the curve? Because "RecorderLogic" is "all", but "FocusCurveRecorderT" really just is the "recorder" an nothing else.



    // TODO...
    mFocusCurveViewPanel->update();
}

void FocusCurveRecorderDialogT::onFocusCurveRecorderFinished(
        const std::shared_ptr<const FocusCurveRecordSetContainerT>& /*focusCurveRecordSetContainer*/) {
    LOG(debug)
        << "FocusCurveRecorderDialogT::onFocusCurveRecorderFinished..." << std::endl;

    mFocusCurveRecordButton->stopAnimation();

    // NOTE: setChecked(false) does not work for some reason...
    mFocusCurveRecordButton->setEnabled(false);
    mFocusCurveRecordButton->setChecked(false);
    mFocusCurveRecordButton->setEnabled(true);

    //    updateFocusFinderMainMenuBar();


    // TODO: Call code below from panel instead..?!!
    //mFocusRecorder->fitFocusCurve(focusCurveRecordSet);

    mFocusCurveViewPanel->update();

    mFocusCurveRecorderProgressDetailsPanel->stopAnimation();

    getOkButton()->setEnabled(true);
    getApplyButton()->setEnabled(true);
}

void FocusCurveRecorderDialogT::onFocusCurveRecorderNewRecord(const std::shared_ptr<FocusCurveRecordT>& focusCurveRecord) {
    LOG(debug)
        << "FocusCurveRecorderDialogT::onFocusCurveRecorderNewRecord..." << std::endl;

    //mFocusCurveViewPanel->update();

    mFocusCurveRecorderProgressDetailsPanel->setCurrentFocusCurveRecord(focusCurveRecord);
}

void FocusCurveRecorderDialogT::onFocusCurveRecorderRecordSetUpdate(
        const std::shared_ptr<FocusCurveRecordSetT>& /*focusCurveRecordSet*/) {
    LOG(debug)
        << "FocusCurveRecorderDialogT::onFocusCurveRecorderRecordSetUpdate..." << std::endl;

    // TODO: Refactor...
    //mFocusCurveViewPanel->setFocusCurve(focusCurveRecordSet);
    mFocusCurveViewPanel->update();
}


void FocusCurveRecorderDialogT::onFocusCurveRecorderProgressUpdate(float progress, const QString &msg,
                                                                   std::shared_ptr<FocusCurveRecordT> focusCurveRecord) {
    LOG(debug) << focusCurveRecord << std::endl;

    // TODO: Somewhere else...
    //mFocusCurveViewPanel->setFocusCurve(focusCurveRecord);

    mFocusCurveRecorderProgressDetailsPanel->setCurrentIterationProgress((int) progress);
    mFocusCurveRecorderProgressDetailsPanel->setCurrentIterationProgressText(msg);
    mFocusCurveRecorderProgressDetailsPanel->setCurrentFocusCurveRecord(focusCurveRecord);
}

void FocusCurveRecorderDialogT::onFocusCurveRecorderCancelled() {
    LOG(debug)
        << "FocusCurveRecorderDialogT::onFocusCurveRecorderCancelled..." << std::endl;

    mFocusCurveRecordButton->stopAnimation();

    // NOTE: setChecked(false) does not work for some reason...
    mFocusCurveRecordButton->setEnabled(false);
    mFocusCurveRecordButton->setChecked(false);
    mFocusCurveRecordButton->setEnabled(true);

    //    updateFocusFinderMainMenuBar();
    mFocusCurveRecorderProgressDetailsPanel->stopAnimation();

    getOkButton()->setEnabled(true);
    getApplyButton()->setEnabled(true);
}

void FocusCurveRecorderDialogT::onFocusCurveRecorderStarted() {
    LOG(debug)
        << "FocusCurveRecorderDialogT::onFocusCurveRecorderStarted..." << std::endl;

    getOkButton()->setEnabled(false);
    getApplyButton()->setEnabled(false);

    mFocusCurveRecordButton->startAnimation();
    mFocusCurveRecorderProgressDetailsPanel->startAnimation();
}

void FocusCurveRecorderDialogT::createFocusCurveRecordButton() {
    mFocusCurveRecordButton = new AnimMenuButtonT;
    mFocusCurveRecordButton->setCheckable(true);

    setBtnIcon(mFocusCurveRecordButton, ":/res/record_focus_curve_64x64.png");

    connect(mFocusCurveRecordButton, &QToolButton::clicked, this,
            &FocusCurveRecorderDialogT::onFocusCurveRecordPressed);

    getMainToolBar()->addWidget(mFocusCurveRecordButton);

    //setFocusCurveRecordButtonState(false);

    // Connect focus curve recorder events with exposure button
    connect(this, &FocusCurveRecorderDialogT::focusCurveRecorderStartedSignal, this,
            &FocusCurveRecorderDialogT::onFocusCurveRecorderStarted);

    connect(this, &FocusCurveRecorderDialogT::focusCurveRecorderNewRecordSignal, this,
            &FocusCurveRecorderDialogT::onFocusCurveRecorderNewRecord);

    connect(this, &FocusCurveRecorderDialogT::focusCurveRecorderRecordSetUpdateSignal, this,
            &FocusCurveRecorderDialogT::onFocusCurveRecorderRecordSetUpdate);

    connect(this, &FocusCurveRecorderDialogT::focusCurveRecorderProgressUpdateSignal, this,
            &FocusCurveRecorderDialogT::onFocusCurveRecorderProgressUpdate);

    connect(this, &FocusCurveRecorderDialogT::focusCurveRecorderRecordSetFinishedSignal, this,
            &FocusCurveRecorderDialogT::onFocusCurveRecorderRecordSetFinished);

    connect(this, &FocusCurveRecorderDialogT::focusCurveRecorderFinishedSignal, this,
            &FocusCurveRecorderDialogT::onFocusCurveRecorderFinished);

    connect(this, &FocusCurveRecorderDialogT::focusCurveRecorderCancelledSignal, this,
            &FocusCurveRecorderDialogT::onFocusCurveRecorderCancelled);


    // TODO: Handle camera disconnect (see main_window...)
}

void FocusCurveRecorderDialogT::createMainToolBar() {
    createFocusCurveRecordButton();

    // QSpacerItem * spacer = new QSpacerItem(10, 10);
    // getFocusFinderMainMenuBar()->addItem(spacer);

    getMainToolBar()->addStretch();
}

QHBoxLayout *FocusCurveRecorderDialogT::getMainToolBar() {
    return m_ui->layMainToolBar;
}
