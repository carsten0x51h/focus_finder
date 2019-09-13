#include <memory>

#include <QWidget>
#include <QHBoxLayout>
#include <QAbstractButton>

#include "include/focus_curve_recorder_panel.h"

#include "../common/include/logging.h"

#include "../common/include/task_executor.h"
#include "../common/include/focus_curve_recorder.h"
#include "../common/include/focus_curve_recorder_type.h"
#include "../common/include/focus_curve_recorder_factory.h"

#include "ui_focus_curve_recorder_panel.h"


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

FocusCurveRecorderPanelT::FocusCurveRecorderPanelT(QWidget * parent, FocusFinderLogicT & ffl) : QWidget(parent),
                                                                                        m_ui(new Ui::FocusCurveRecorderPanel),
                                                                                        mFfl(ffl)
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
    mRecorderExec = std::make_shared<TaskExecutorT<FocusCurveRecorderT> > (); // WAS: mFfl.getFocusFinderExecutor();

    reset();
}

FocusCurveRecorderPanelT::~FocusCurveRecorderPanelT()
{
  delete mFocusCurveRecordButton; // TODO: Ok?
}

void FocusCurveRecorderPanelT::reset() {

}

// TODO: Maybe this should be done inside the FocusCurveRecorderT?
bool FocusCurveRecorderPanelT::deviceCheck() const {
  auto activeCamera = mFfl.getCurrentCamera();
  auto activeFocuser = mFfl.getCurrentCamera();
  
  if (!activeCamera) {
    LOG(warning)
      << "No camera set!" << std::endl;
    return false;
  }
  
  if (!activeFocuser) {
    LOG(warning)
      << "No focuser set!" << std::endl;
    return false;
  }
  return true;
}

void FocusCurveRecorderPanelT::onFocusCurveRecordPressed(bool isChecked) {
  LOG(debug) << "FocusCurveRecorderPanelT::onFocusCurveRecordPressed... mFocusCurveRecordButton->isChecked(): " << isChecked << std::endl;

  if (isChecked) {

    if (!deviceCheck()) {
      // TODO: Log error / warning...
      return;
    }

    
    // TODO: Check that ffExec it is not yet running?

    // TODO: Check that every device is connected?

    // TODO: Always create a new focus finder instance? Otherwise.. where to store?
    //       -> depends on selection... -> FocusFinder profile...

    // HACK / TODO: For now we create always a new instance...
    // TODO: Do not create here?! -> Factory?

    auto focusCurveRecorder = FocusCurveRecorderFactoryT::getInstance(
							FocusCurveRecorderTypeT::DEFAULT);

    // Set devices
    // TODO: Implement
    // focusCurveRecorder->setCamera(mFfl.getCurrentCamera());
    // focusCurveRecorder->setFocus(mFfl.getCurrentFocus());
    // focusCurveRecorder->setFilter(mFfl.getCurrentFilter());

    //auto activeProfileOpt = mFfl.getProfileManager()->getActiveProfile();

    // TODO: Make sure that activeProfile is set...
    //		if (!activeProfileOpt) {
    //		}
    // TODO: Implement
    //focusCurveRecorder->setFocusFinderProfile(activeProfileOpt.value());

    
    // Register FocusCurveRecorder UI listeners
    // FocusCurveRecorder started
    focusCurveRecorder->registerFocusCurveRecorderStartedListener([&]() {
    						      emit focusCurveRecorderStartedSignal();
    						    });

    // FoFi running / status update
    // qRegisterMetaType < std::shared_ptr<FocusFinderRecordT> > ("FocusFinderRecordPtrT");
    
    // focusFinder->registerFocusFinderProgressUpdateListener(
    // 							   [&](float progress, const std::string & msg, std::shared_ptr<FocusFinderRecordT> record) {
    // 							     emit focusFinderProgressUpdateSignal(progress, QString::fromStdString(msg), record);
    // 							   });

    // FocusCurveRecorder finished
    focusCurveRecorder->registerFocusCurveRecorderFinishedListener([&](bool lastCurve) {
    						       emit focusCurveRecorderFinishedSignal(lastCurve);
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

void FocusCurveRecorderPanelT::onFocusCurveRecorderFinished(bool isLastCurve) {
	LOG(debug)
	<< "FocusCurveRecorderPanelT::onFocusCurveRecorderFinished...lastCurve=" << isLastCurve
			<< std::endl;

	if (isLastCurve) {
	  mFocusCurveRecordButton->stopAnimation();

	  // NOTE: setChecked(false) does not work for some reason...
	  mFocusCurveRecordButton->setEnabled(false);
	  mFocusCurveRecordButton->setChecked(false);
	  mFocusCurveRecordButton->setEnabled(true);
	}
	//    updateFocusFinderMainMenuBar();
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
