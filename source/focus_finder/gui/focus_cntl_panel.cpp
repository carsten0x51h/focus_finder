#include <memory>
#include <cmath>

#include <QMovie>

#include "include/focus_cntl_panel.h"

#include "../common/include/logging.h"
#include "../common/include/device_manager.h"
#include "../common/include/profile_manager.h"
#include "../common/include/focus.h"

#include "ui_focus_cntl_panel.h"

FocusCntlPanelT::FocusCntlPanelT(QWidget * parent, FocusFinderLogicT & ffl) : QWidget(parent),
m_ui(new Ui::FocusCntlPanel),
mMovie(nullptr),
mFfl(ffl),
mFocusDevice(nullptr)
{
	// Setup UI
	m_ui->setupUi(this);

	mMovie = new QMovie(QStringLiteral(":/res/wait3.gif"));

	connect(mMovie, SIGNAL(frameChanged(int)), this, SLOT(setFocusAnimationIcon(int)));

	// if movie doesn't loop forever, force it to.
	// TODO: Update to new signal-slot API...
	if (mMovie->loopCount() != -1) {
		connect(mMovie, SIGNAL(finished()), mMovie, SLOT(start()));
	}

	mStepSignalMapper = new QSignalMapper(this);

	connect (m_ui->btnPlus1000, SIGNAL(clicked()), mStepSignalMapper, SLOT(map()));
	connect (m_ui->btnPlus100, SIGNAL(clicked()), mStepSignalMapper, SLOT(map()));
	connect (m_ui->btnPlus10, SIGNAL(clicked()), mStepSignalMapper, SLOT(map()));
	connect (m_ui->btnPlus1, SIGNAL(clicked()), mStepSignalMapper, SLOT(map()));
	connect (m_ui->btnMinus1000, SIGNAL(clicked()), mStepSignalMapper, SLOT(map()));
	connect (m_ui->btnMinus100, SIGNAL(clicked()), mStepSignalMapper, SLOT(map()));
	connect (m_ui->btnMinus10, SIGNAL(clicked()), mStepSignalMapper, SLOT(map()));
	connect (m_ui->btnMinus1, SIGNAL(clicked()), mStepSignalMapper, SLOT(map()));

	mStepSignalMapper->setMapping(m_ui->btnPlus1000, 1000);
	mStepSignalMapper->setMapping(m_ui->btnPlus100, 100);
	mStepSignalMapper->setMapping(m_ui->btnPlus10, 10);
	mStepSignalMapper->setMapping(m_ui->btnPlus1, 1);
	mStepSignalMapper->setMapping(m_ui->btnMinus1000, -1000);
	mStepSignalMapper->setMapping(m_ui->btnMinus100, -100);
	mStepSignalMapper->setMapping(m_ui->btnMinus10, -10);
	mStepSignalMapper->setMapping(m_ui->btnMinus1, -1);

	connect(mStepSignalMapper, SIGNAL(mapped(int)), this, SLOT(onFocusPositionChangeRequest(int)));

	connect(m_ui->btnAbort, & QPushButton::pressed, this, & FocusCntlPanelT::onFocusAbortRequest);

	// Connect UI to focus events
	connect(this, & FocusCntlPanelT::deviceConnectedSignal, this, & FocusCntlPanelT::onDeviceConnectedSlot);
	connect(this, & FocusCntlPanelT::deviceDisconnectedSignal, this, & FocusCntlPanelT::onDeviceDisconnectedSlot);
	connect(this, & FocusCntlPanelT::focusPositionChangedSignal, this, & FocusCntlPanelT::onFocusPositionChangedSlot);
	connect(this, & FocusCntlPanelT::targetPositionReachedSignal, this, & FocusCntlPanelT::onTargetPositionReachedSlot);
	connect(this, & FocusCntlPanelT::focusMovementAbortedSignal, this, & FocusCntlPanelT::onFocusMovementAbortedSlot);

	updateProfile();

	// Register at profile manager to get notified if selected profile / device changes...
	mFfl.getProfileManager()->registerActiveProfileChangedListener(
			std::bind(&FocusCntlPanelT::updateProfile, this));
}

FocusCntlPanelT::~FocusCntlPanelT() {
	// TODO: unregister from device?

	delete mStepSignalMapper;
	delete mMovie;
}

void FocusCntlPanelT::setFocusAnimationIcon(int /*frame*/) {
	m_ui->lblFocusAnimation->setPixmap(mMovie->currentPixmap());
}

void FocusCntlPanelT::startAnimation() {
	LOG(debug) << "FocusCntlPanelT::startAnimation..." << std::endl;

	mMovie->start();
}

void FocusCntlPanelT::stopAnimation() {
	LOG(debug) << "FocusCntlPanelT::stopAnimation..." << std::endl;

	mMovie->stop();
}

void FocusCntlPanelT::onDeviceConnectedSlot() {
	LOG(debug) << "FocusCntlPanelT::onDeviceConnectedSlot..." << std::endl;
	this->setEnabled(true);
}

void FocusCntlPanelT::onDeviceDisconnectedSlot() {
	LOG(debug) << "FocusCntlPanelT::onDeviceDisconnectedSlot..."
			<< std::endl;
	this->setEnabled(false);
}

void FocusCntlPanelT::onFocusPositionChangedSlot(int absPos) {
	LOG(debug) << "FocusCntlPanelT::onFocusPositionChangedSlot...absPos: "
			<< absPos << std::endl;

	updateCurrentPos(absPos);
}

void FocusCntlPanelT::onTargetPositionReachedSlot(int absPos) {
	LOG(debug) << "FocusCntlPanelT::onTargetPositionReachedSlot...absPos: "
			<< absPos << std::endl;
	stopAnimation();

	updateCurrentPos(absPos);
}

void FocusCntlPanelT::onFocusMovementAbortedSlot(int currentPos) {
	LOG(debug)
			<< "FocusCntlPanelT::onFocusMovementAbortedSlot...currentPos: "
			<< currentPos << std::endl;
	stopAnimation();
	// TODO: Re-enable Abort button?!

	updateCurrentPos(currentPos);
}

void FocusCntlPanelT::updateCurrentPos(int absPos) {
	m_ui->lblIsPosition->setText(
			QString::fromStdString(std::to_string(absPos)));

	m_ui->progressBarAbsPos->setValue(absPos);
}

void FocusCntlPanelT::updateProfile() {

	auto newFocusDevice = mFfl.getCurrentFocus();
	bool hasOldFocus = (mFocusDevice != nullptr);
	bool enablePanel = (
			newFocusDevice ?
					newFocusDevice->getConnector()->isConnected() : false);

	this->setEnabled(enablePanel);

	if (mFocusDevice == newFocusDevice) {
		return;
	}

	if (hasOldFocus) {
		// There was already an old device - unregister listener and register to the new one.
		mFocusDevice->getConnector()->unregisterDeviceConnectedListener(
				mDeviceConnectedConnection);
		mFocusDevice->getConnector()->unregisterDeviceDisconnectedListener(
				mDeviceDisconnectedConnection);
		mFocusDevice->unregisterFocusPositionChangedListener(
				mFocusPositionChangedConnection);
		mFocusDevice->unregisterTargetPositionReachedListener(
				mTargetPositionReachedConnection);
		mFocusDevice->unregisterFocusMovementAbortedListener(
				mFocusMovementAbortedConnection);
	}

	// Register to new device
	if (newFocusDevice) {
		mDeviceConnectedConnection =
				newFocusDevice->getConnector()->registerDeviceConnectedListener(
						[&]() {emit deviceConnectedSignal();});
		mDeviceDisconnectedConnection =
				newFocusDevice->getConnector()->registerDeviceDisconnectedListener(
						[&]() {emit deviceDisconnectedSignal();});
		mFocusPositionChangedConnection =
				newFocusDevice->registerFocusPositionChangedListener(
						[&](int absPos) {emit focusPositionChangedSignal(absPos);});
		mTargetPositionReachedConnection =
				newFocusDevice->registerTargetPositionReachedListener(
						[&](int absPos) {emit targetPositionReachedSignal(absPos);});
		mFocusMovementAbortedConnection =
				newFocusDevice->registerFocusMovementAbortedListener(
						[&](int currentPos) {emit focusMovementAbortedSignal(currentPos);});

		// Read important focus parameters
		try {
			int minPos = newFocusDevice->getMinAbsPos();
			int maxPos = newFocusDevice->getMaxAbsPos();

			m_ui->progressBarAbsPos->setRange(minPos, maxPos);

		} catch (FocusExceptionT & exc) {
			// TODO: Further handling required?
			LOG(error) << "Unable to determine absolute focus boundaries!"
					<< std::endl;
		}

		try {

			int currentPos = newFocusDevice->getCurrentPos();

			updateCurrentPos(currentPos);

		} catch (FocusExceptionT & exc) {
			// TODO: Further handling required?
			LOG(error)
					<< "Unable to determine current absolute focus position!"
					<< std::endl;
		}
	}

	mFocusDevice = newFocusDevice;
}

void FocusCntlPanelT::onFocusPositionChangeRequest(int inSteps) {
	LOG(debug) << "FocusCntlPanelT::onFocusPositionChangeRequest...inSteps="
			<< inSteps << std::endl;

	startAnimation();

	FocusDirectionT::TypeE direction = (
			inSteps < 0 ? FocusDirectionT::INWARD : FocusDirectionT::OUTWARD);
	mFocusDevice->setTargetPos(std::abs(inSteps), direction);


	m_ui->lblTgtPosition->setText(
			QString::fromStdString(
					std::to_string(mFocusDevice->getTargetPos())));
}

void FocusCntlPanelT::onFocusAbortRequest() {
	LOG(debug) << "FocusCntlPanelT::onFocusAbortRequest..." << std::endl;
	mFocusDevice->abortMotion();
}
