#include <memory>

#include <QWidget>
#include <QHBoxLayout>
#include <QAbstractButton>

#include "include/focus_curve_recorder_panel.h"
#include "include/focus_curve_view_panel.h"

#include "../common/include/logging.h"
#include "../common/include/profile_manager.h"

#include "../common/include/task_executor.h"
#include "../common/include/focus_curve_recorder.h"
#include "../common/include/focus_curve_record.h"
#include "../common/include/focus_curve_recorder_type.h"
#include "../common/include/focus_curve_recorder_factory.h"
#include "../common/include/focus_curve_recorder_logic.h"
#include "../common/include/focus_curve_record_set.h"


#include "ui_focus_curve_recorder_panel.h"

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
    
    
    reset();
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

    
    // TODO: Check that ffExec it is not yet running?

    // TODO: Check that every device is connected?

    // TODO: Always create a new focus finder instance? Otherwise.. where to store?
    //       -> depends on selection... -> FocusFinder profile...

    // HACK / TODO: For now we create always a new instance...
    // TODO: Do not create here?! -> Factory?

    mFocusCurveRecorderLogic->resetFocusCurveRecorder(FocusCurveRecorderTypeT::DEFAULT);
    
    auto focusCurveRecorder = mFocusCurveRecorderLogic->getFocusCurveRecorder();

    // TODO: Get value from UI
    focusCurveRecorder->setFocusMeasureType(FocusMeasureTypeT::HFD);

    // TODO: Can FocusCurveViewPanelT extract this info from "focusCurveRecorder" instead?
    mFocusCurveViewPanel->setFocusMeasureType(FocusMeasureTypeT::HFD);


    
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
}

void FocusCurveRecorderPanelT::onFocusCurveRecorderStarted() {
	LOG(debug)
	<< "FocusCurveRecorderPanelT::onFocusCurveRecorderStarted..." << std::endl;
	mFocusCurveRecordButton->startAnimation();
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
