#include <memory>

#include <QWidget>
#include <QHBoxLayout>
#include <QAbstractButton>

#include "include/focus_curve_recorder_panel.h"
#include "include/focus_curve_view_panel.h"
#include "include/focus_curve_recorder_progress_details_panel.h"
#include "include/focus_curve_recorder_curve_details_panel.h"
#include "include/focus_curve_recorder_point_details_panel.h"
#include "include/focus_curve_recorder_summary_details_panel.h"

#include "../common/include/logging.h"
#include "../common/include/profile_manager.h"

#include "../common/include/task_executor.h"
#include "../common/include/focus_curve_recorder.h"
#include "../common/include/focus_curve_record.h"
#include "../common/include/focus_curve_recorder_type.h"
#include "../common/include/focus_curve_recorder_factory.h"
#include "../common/include/focus_curve_recorder_logic.h"
#include "../common/include/focus_curve_record_set.h"
#include "../common/include/focus_measure_type.h"
#include "../common/include/fitting_curve_type.h"

#include "ui_focus_curve_recorder_panel.h"

Q_DECLARE_METATYPE(FocusMeasureTypeT::TypeE)
Q_DECLARE_METATYPE(FittingCurveTypeT::TypeE)
Q_DECLARE_METATYPE(std::shared_ptr<FocusCurveRecordSetT>)



// TODO: Actually this is duplicated code - see main_window.cpp...
void FocusCurveRecorderPanelT::setBtnIcon(QAbstractButton * btn,
		const std::string & filename) {
	QIcon icon;

	icon.addFile(QString::fromStdString(filename), QSize(), QIcon::Normal,
			QIcon::Off);

	btn->setIcon(icon);
	btn->setMinimumSize(QSize(48, 48));
	btn->setIconSize(QSize(48, 48));
}

FocusCurveRecorderPanelT::FocusCurveRecorderPanelT(QWidget * parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic) : QWidget(parent),
                                                                                        m_ui(new Ui::FocusCurveRecorderPanel),
                                                                                        mFocusCurveRecorderLogic(focusCurveRecorderLogic)
{
    // Setup UI
    m_ui->setupUi(this);

    createMainToolBar();
//    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    sizePolicy.setHorizontalStretch(100);
//    sizePolicy.setVerticalStretch(100);
//
//    mFocusCurveWidget = new FocusCurveViewWidgetT(m_ui->widget);
//    mFocusCurveWidget->setSizePolicy(sizePolicy);
//    m_ui->layFocusCurveViewWidget->addWidget(mFocusCurveWidget, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);

    // TODO: Move to reset?
    mRecorderExec = std::make_shared<TaskExecutorT<FocusCurveRecorderT> > ();

    // Add the focus curve viewer panel
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(100);
    sizePolicy.setVerticalStretch(100);

    mFocusCurveViewPanel = new FocusCurveViewPanelT(m_ui->widget, mFocusCurveRecorderLogic);
    mFocusCurveViewPanel->setSizePolicy(sizePolicy);
    m_ui->layFocusCurveViewPanel->addWidget(mFocusCurveViewPanel, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);
    

    // Configure setting elements
    /////////////////////////////

    initFocusCurveTypeCombobox();

    // Add progress details panel
    mFocusCurveRecorderSummaryDetailsPanel = new FocusCurveRecorderSummaryDetailsPanelT(m_ui->wgtDetailsWidget, focusCurveRecorderLogic);
    m_ui->layDetailsWidget->addWidget(mFocusCurveRecorderSummaryDetailsPanel, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);

    mFocusCurveRecorderProgressDetailsPanel = new FocusCurveRecorderProgressDetailsPanelT(m_ui->wgtDetailsWidget, focusCurveRecorderLogic);
    m_ui->layDetailsWidget->addWidget(mFocusCurveRecorderProgressDetailsPanel, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);
    mFocusCurveRecorderProgressDetailsPanel->setVisible(false);
    
    mFocusCurveRecorderCurveDetailsPanel = new FocusCurveRecorderCurveDetailsPanelT(m_ui->wgtDetailsWidget, focusCurveRecorderLogic);
    m_ui->layDetailsWidget->addWidget(mFocusCurveRecorderCurveDetailsPanel, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);
    mFocusCurveRecorderCurveDetailsPanel->setVisible(false);
    
    mFocusCurveRecorderPointDetailsPanel = new FocusCurveRecorderPointDetailsPanelT(m_ui->wgtDetailsWidget, focusCurveRecorderLogic);
    m_ui->layDetailsWidget->addWidget(mFocusCurveRecorderPointDetailsPanel, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);
    mFocusCurveRecorderPointDetailsPanel->setVisible(false);
    
    //mFocusCurveRecorderProgressDetailsPanel->setParent(m_ui->wgtDetailsWidget);
    //mFocusCurveRecorderCurveDetailsPanel->setParent(m_ui->wgtDetailsWidget);
    //mFocusCurveRecorderProgressDetailsPanel->startAnimation();


    
    // HACK - REMOVE LATER!
    connect(m_ui->pushButton, & QPushButton::pressed, this, & FocusCurveRecorderPanelT::onPushButtonPressed);

    
    reset();
}

void FocusCurveRecorderPanelT::selectDetailView(FocusCurveRecorderDetailViewE detailView)
{
  LOG(debug) << "FocusCurveRecorderPanelT::onPushButtonPressed... detailView=" << detailView << std::endl;

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
    LOG(error) << "Invalid detailView " << detailView  << ". Not changing the view!" << std::endl;
    break;
  }
}

void FocusCurveRecorderPanelT::onPushButtonPressed()
{
  static int idx = 0;
  
  idx = (++idx) % 4;

  FocusCurveRecorderDetailViewE detailView = static_cast<FocusCurveRecorderDetailViewE>(idx);
  selectDetailView(detailView);  
}


void FocusCurveRecorderPanelT::addFocusCurveType(FittingCurveTypeT::TypeE focusCurveType) {
  QVariant data;
  data.setValue(focusCurveType);
  
  m_ui->cbxFocusCurveType->addItem(
				   QString::fromStdString(FittingCurveTypeT::asStr(focusCurveType)),
				   data
				   );
}

void FocusCurveRecorderPanelT::initFocusCurveTypeCombobox() {

  addFocusCurveType(FittingCurveTypeT::HYPERBOLIC);
  addFocusCurveType(FittingCurveTypeT::PARABOLIC);
  
  connect(m_ui->cbxFocusCurveType, QOverload<int>::of(&QComboBox::currentIndexChanged),
    		this, &FocusCurveRecorderPanelT::onFocusCurveTypeSelectionChanged);
}

void FocusCurveRecorderPanelT::onFocusCurveTypeSelectionChanged() {
  QVariant cbxData = m_ui->cbxFocusCurveType->currentData();
  auto focusCurveType = cbxData.value<FittingCurveTypeT::TypeE>();

  LOG(debug) << "FocusCurveRecorderPanelT::onFocusCurveTypeSelectionChanged...focusCurveType=" << FittingCurveTypeT::asStr(focusCurveType) << std::endl;

  // TODO...
}


FocusCurveRecorderPanelT::~FocusCurveRecorderPanelT()
{
  delete mFocusCurveRecordButton; // TODO: Ok?
}

void FocusCurveRecorderPanelT::reset() {

}


void FocusCurveRecorderPanelT::onFocusCurveRecordPressed(bool isChecked) {
  LOG(debug) << "FocusCurveRecorderPanelT::onFocusCurveRecordPressed... mFocusCurveRecordButton->isChecked(): " << isChecked << std::endl;

  if (isChecked) {

    if (!mFocusCurveRecorderLogic->checkDevices()) {
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

    // HACK / TODO: For now we create always a new instance...
    // TODO: Do not create here?! -> Factory?
    mFocusCurveRecorderLogic->resetFocusCurveRecorder(FocusCurveRecorderTypeT::DEFAULT);
    
    auto focusCurveRecorder = mFocusCurveRecorderLogic->getFocusCurveRecorder();

    // Register FocusCurveRecorder UI listeners
    // FocusCurveRecorder started
    focusCurveRecorder->registerFocusCurveRecorderStartedListener([&]() {
    						      emit focusCurveRecorderStartedSignal();
    						    });

    // Register listener to get notified when there is a new record available
    qRegisterMetaType < std::shared_ptr<FocusCurveRecordT> > ("FocusCurveRecordPtrT");

    focusCurveRecorder->registerFocusCurveRecorderNewRecordListener(
								    [&](std::shared_ptr<FocusCurveRecordT> record) {
								      emit focusCurveRecorderNewRecordSignal(record);
    							   });

    // Register listener to get notified when the focus curve record set was updated (new data point for curve recorded)
    qRegisterMetaType < std::shared_ptr<FocusCurveRecordSetT> > ("FocusCurveRecordSetPtrT");

    focusCurveRecorder->registerFocusCurveRecorderRecordSetUpdateListener(
								    [&](std::shared_ptr<FocusCurveRecordSetT> recordSet) {
								      emit focusCurveRecorderRecordSetUpdateSignal(recordSet);
    							   });

    
    // Register progress listener
    focusCurveRecorder->registerFocusCurveRecorderProgressUpdateListener(
									 [&](float progress, const std::string & msg, std::shared_ptr<FocusCurveRecordT> record) {
    							     emit focusCurveRecorderProgressUpdateSignal(progress, QString::fromStdString(msg), record);
    							   });

    
    // FocusCurveRecorder new focus curve record set finished
    focusCurveRecorder->registerFocusCurveRecorderRecordSetFinishedListener([&](std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet) {
    						       emit focusCurveRecorderRecordSetFinishedSignal(focusCurveRecordSet);
    						     });

    // FocusCurveRecorder finished
    qRegisterMetaType < std::shared_ptr<const FocusCurveRecordSetContainerT> > ("FocusCurveRecordSetContainerPtrT");

    focusCurveRecorder->registerFocusCurveRecorderFinishedListener([&](std::shared_ptr<const FocusCurveRecordSetContainerT> focusCurveRecordSetContainer) {
    						       emit focusCurveRecorderFinishedSignal(focusCurveRecordSetContainer);
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

// void FocusCurveRecorderPanelT::setFocusCurveRecordButtonState(bool isRunning) {
// 	LOG(debug)
// 	<< "FocusCurveRecorderPanelT::setFocusCurveRecordButtonState...isRunning=" << isRunning
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
void FocusCurveRecorderPanelT::onFocusCurveRecorderRecordSetFinished(std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet) {
	LOG(debug)
	<< "FocusCurveRecorderPanelT::onFocusCurveRecorderRecordSetFinished..." << std::endl;
	
	// Match the curve...
	auto focusCurve = std::make_shared<FocusCurveT>(focusCurveRecordSet, mFocusCurveRecorderLogic->getFocusCurveType());
	//TODO: What happens to focusCurve?? -> needs to be drawn......
	  
	// TODO: Currently we do not cache the calculatef FocusCurve.
	//       We will find out if this will be required....
	//       If it needs to be stored, we store it on the
	//       FocusCurveRecoderLogic class.
	// TODO:  Store focusCurve in RecorderLogic? Connection to RecordSet? Triggered by this listener? Guess Logic class should have own listener!.. and then calculate the curve? Because "RecorderLogic" is "all", but "FocusCurveRecorderT" really just is the "recorder" an nothing else.

	
	
	// TODO...
	mFocusCurveViewPanel->update();
}

void FocusCurveRecorderPanelT::onFocusCurveRecorderFinished(std::shared_ptr<const FocusCurveRecordSetContainerT> focusCurveRecordSetContainer) {
	LOG(debug)
	<< "FocusCurveRecorderPanelT::onFocusCurveRecorderFinished..." << std::endl;

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
}

void FocusCurveRecorderPanelT::onFocusCurveRecorderNewRecord(std::shared_ptr<FocusCurveRecordT> focusCurveRecord) {
	LOG(debug)
	  << "FocusCurveRecorderPanelT::onFocusCurveRecorderNewRecord..." << std::endl;
	
	//mFocusCurveViewPanel->update();
}

void FocusCurveRecorderPanelT::onFocusCurveRecorderRecordSetUpdate(std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet) {
	LOG(debug)
	  << "FocusCurveRecorderPanelT::onFocusCurveRecorderRecordSetUpdate..." << std::endl;

	// TODO: Refactor...
	//mFocusCurveViewPanel->setFocusCurve(focusCurveRecordSet);
	mFocusCurveViewPanel->update();
}


void FocusCurveRecorderPanelT::onFocusCurveRecorderProgressUpdate(float progress, const QString & msg, std::shared_ptr<FocusCurveRecordT> focusCurveRecord) {
  LOG(debug) << focusCurveRecord << std::endl;

  // TODO: Somewhere else...
  //mFocusCurveViewPanel->setFocusCurve(focusCurveRecord);

  mFocusCurveRecorderProgressDetailsPanel->setCurrentIterationProgress(progress);
  mFocusCurveRecorderProgressDetailsPanel->setCurrentIterationProgressText(msg);
}

void FocusCurveRecorderPanelT::onFocusCurveRecorderCancelled() {
	LOG(debug)
	<< "FocusCurveRecorderPanelT::onFocusCurveRecorderCancelled..." << std::endl;

	mFocusCurveRecordButton->stopAnimation();

	// NOTE: setChecked(false) does not work for some reason...
	mFocusCurveRecordButton->setEnabled(false);
	mFocusCurveRecordButton->setChecked(false);
	mFocusCurveRecordButton->setEnabled(true);

	//    updateFocusFinderMainMenuBar();
	mFocusCurveRecorderProgressDetailsPanel->stopAnimation();
}

void FocusCurveRecorderPanelT::onFocusCurveRecorderStarted() {
	LOG(debug)
	<< "FocusCurveRecorderPanelT::onFocusCurveRecorderStarted..." << std::endl;
	mFocusCurveRecordButton->startAnimation();
	mFocusCurveRecorderProgressDetailsPanel->startAnimation();
}

void FocusCurveRecorderPanelT::createFocusCurveRecordButton() {
  mFocusCurveRecordButton = new AnimMenuButtonT;
  mFocusCurveRecordButton->setCheckable(true);

  setBtnIcon(mFocusCurveRecordButton, ":/res/record_focus_curve_64x64.png");

  connect(mFocusCurveRecordButton, &QToolButton::clicked, this,
	  &FocusCurveRecorderPanelT::onFocusCurveRecordPressed);
  
  getMainToolBar()->addWidget(mFocusCurveRecordButton);

  //setFocusCurveRecordButtonState(false);

  // Connect focus curve recorder events with exposure button
  connect(this, &FocusCurveRecorderPanelT::focusCurveRecorderStartedSignal, this,
	  &FocusCurveRecorderPanelT::onFocusCurveRecorderStarted);

  connect(this, &FocusCurveRecorderPanelT::focusCurveRecorderNewRecordSignal, this,
	  &FocusCurveRecorderPanelT::onFocusCurveRecorderNewRecord);

  connect(this, &FocusCurveRecorderPanelT::focusCurveRecorderRecordSetUpdateSignal, this,
	  &FocusCurveRecorderPanelT::onFocusCurveRecorderRecordSetUpdate);
  
  connect(this, &FocusCurveRecorderPanelT::focusCurveRecorderProgressUpdateSignal, this,
	  &FocusCurveRecorderPanelT::onFocusCurveRecorderProgressUpdate);
  
  connect(this, &FocusCurveRecorderPanelT::focusCurveRecorderRecordSetFinishedSignal, this,
	  &FocusCurveRecorderPanelT::onFocusCurveRecorderRecordSetFinished);
 
  connect(this, &FocusCurveRecorderPanelT::focusCurveRecorderFinishedSignal, this,
	  &FocusCurveRecorderPanelT::onFocusCurveRecorderFinished);
  
  connect(this, &FocusCurveRecorderPanelT::focusCurveRecorderCancelledSignal, this,
	  &FocusCurveRecorderPanelT::onFocusCurveRecorderCancelled);
  
  
  // TODO: Handle camera disconnect (see main_window...)
}

void FocusCurveRecorderPanelT::createMainToolBar() {
  createFocusCurveRecordButton();
  
  // QSpacerItem * spacer = new QSpacerItem(10, 10);
  // getFocusFinderMainMenuBar()->addItem(spacer);
  
  getMainToolBar()->addStretch();
}

QHBoxLayout * FocusCurveRecorderPanelT::getMainToolBar() {
  return m_ui->layMainToolBar;
}
