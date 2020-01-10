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

#include <memory>
#include <cmath>

#include <QMovie>

#include "include/filter_cntl_panel.h"

#include "../common/include/logging.h"
#include "../common/include/device_manager.h"
#include "../common/include/profile_manager.h"
#include "../common/include/filter.h"

#include "ui_filter_cntl_panel.h"


FilterCntlPanelT::FilterCntlPanelT(QWidget * parent, FocusFinderLogicT & ffl) : QWidget(parent),
	m_ui(new Ui::FilterCntlPanel),
	mMovie(nullptr),
	mFfl(ffl),
	mFilterDevice(nullptr)
{
    // Setup UI
    m_ui->setupUi(this);

    mMovie = new QMovie(QStringLiteral(":/res/wait3.gif"));

    connect(mMovie, SIGNAL(frameChanged(int)), this, SLOT(setFilterAnimationIcon(int)));

    // if movie doesn't loop forever, force it to.
    // TODO: Update to new signal-slot API...
    if (mMovie->loopCount() != -1) {
    	connect(mMovie, SIGNAL(finished()), mMovie, SLOT(start()));
    }

    connect(m_ui->cbxSelectedFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
    		this, &FilterCntlPanelT::onFilterPositionChangeRequest);

    //connect(m_ui->btnAbort, & QPushButton::pressed, this, & FocusCntlPanelT::onFocusAbortRequest);

    // Connect UI to focus events
    connect(this, & FilterCntlPanelT::deviceConnectedSignal, this, & FilterCntlPanelT::onDeviceConnectedSlot);
    connect(this, & FilterCntlPanelT::deviceDisconnectedSignal, this, & FilterCntlPanelT::onDeviceDisconnectedSlot);
    connect(this, & FilterCntlPanelT::filterPositionChangedSignal, this, & FilterCntlPanelT::onFilterPositionChangedSlot);
    connect(this, & FilterCntlPanelT::targetPositionReachedSignal, this, & FilterCntlPanelT::onTargetPositionReachedSlot);
    connect(this, & FilterCntlPanelT::filterMovementAbortedSignal, this, & FilterCntlPanelT::onFilterMovementAbortedSlot);

	updateProfile();

	// Register at profile manager to get notified if selected profile / device changes...
	mFfl.getProfileManager()->registerActiveProfileChangedListener(
			std::bind(&FilterCntlPanelT::updateProfile, this));
}

FilterCntlPanelT::~FilterCntlPanelT()
{
	delete mMovie;
}

void FilterCntlPanelT::setFilterAnimationIcon(int /*frame*/)
{
    m_ui->lblFilterAnimation->setPixmap(mMovie->currentPixmap());
}

void FilterCntlPanelT::startAnimation() {
	mMovie->start();
}

void FilterCntlPanelT::stopAnimation() {
	mMovie->stop();
}

void FilterCntlPanelT::onDeviceConnectedSlot() {
	LOG(debug) << "FilterCntlPanelT::onDeviceConnectedSlot..." << std::endl;
	this->setEnabled(true);
	m_ui->cbxSelectedFilter->setEnabled(true);
}

void FilterCntlPanelT::onDeviceDisconnectedSlot() {
	LOG(debug) << "FilterCntlPanelT::onDeviceDisconnectedSlot..." << std::endl;
	this->setEnabled(false);
}

void FilterCntlPanelT::onFilterPositionChangedSlot(int currentPos) {
	LOG(debug) << "FilterCntlPanelT::onFilterPositionChangedSlot...currentPos: " << currentPos << std::endl;
	// TODO: Write status text...
	m_ui->lblStatus->setText(QString::fromStdString(std::to_string(currentPos)));
}

void FilterCntlPanelT::onTargetPositionReachedSlot(int targetPos) {
	LOG(debug) << "FilterCntlPanelT::onTargetPositionReachedSlot...targetPos: " << targetPos << std::endl;
	// TODO: Write status text...
	m_ui->lblStatus->setText(QString::fromStdString(std::to_string(targetPos)));
	stopAnimation();
	m_ui->cbxSelectedFilter->setEnabled(true);
}

void FilterCntlPanelT::onFilterMovementAbortedSlot(int currentPos) {
	LOG(debug) << "FilterCntlPanelT::onFilterMovementAbortedSlot...currentPos: " << currentPos << std::endl;
	stopAnimation();
	m_ui->cbxSelectedFilter->setEnabled(true);
}

void FilterCntlPanelT::updateProfile() {

	auto newFilterDevice = mFfl.getCurrentFilter();
	bool hasOldFilter = (mFilterDevice != nullptr);
	bool enablePanel = (newFilterDevice ? newFilterDevice->getConnector()->isConnected() : false);

	this->setEnabled(enablePanel);

	if (mFilterDevice == newFilterDevice) {
		return;
	}

	if (hasOldFilter) {
		// There was already an old device - unregister listener and register to the new one.
		mFilterDevice->getConnector()->unregisterDeviceConnectedListener(mDeviceConnectedConnection);
		mFilterDevice->getConnector()->unregisterDeviceDisconnectedListener(mDeviceDisconnectedConnection);
		mFilterDevice->unregisterFilterPositionChangedListener(mFilterPositionChangedConnection);
		mFilterDevice->unregisterTargetPositionReachedListener(mTargetPositionReachedConnection);
		mFilterDevice->unregisterFilterMovementAbortedListener(mFilterMovementAbortedConnection);

		m_ui->cbxSelectedFilter->clear();
	}

	// Register to new device
	if (newFilterDevice) {
		mDeviceConnectedConnection = newFilterDevice->getConnector()->registerDeviceConnectedListener([&]() { emit deviceConnectedSignal(); });
		mDeviceDisconnectedConnection = newFilterDevice->getConnector()->registerDeviceDisconnectedListener([&]() { emit deviceDisconnectedSignal(); });
		mFilterPositionChangedConnection = newFilterDevice->registerFilterPositionChangedListener([&](int currentPos) { emit filterPositionChangedSignal(currentPos); });
		mTargetPositionReachedConnection = newFilterDevice->registerTargetPositionReachedListener([&](int targetPos) { emit targetPositionReachedSignal(targetPos); });
		mFilterMovementAbortedConnection = newFilterDevice->registerFilterMovementAbortedListener([&](int currentPos) { emit filterMovementAbortedSignal(currentPos); });

		// Fill entries to the combobox
		bool oldState = m_ui->cbxSelectedFilter->blockSignals(true);

		for (int idx = newFilterDevice->getMinPos(); idx != newFilterDevice->getMaxPos(); ++idx) {
			// TODO: Mapping of filter names?! As part of filter device? Or external? How to bind to resp. filter device?
			m_ui->cbxSelectedFilter->addItem(QString::fromStdString(std::to_string(idx)));
		}

		m_ui->cbxSelectedFilter->blockSignals(oldState);
		m_ui->cbxSelectedFilter->setEnabled(true);
	}

	mFilterDevice = newFilterDevice;
}




void FilterCntlPanelT::onFilterPositionChangeRequest(int targetIdx) {
	LOG(debug) << "FilterCntlPanelT::onFilterPositionChangeRequest...targetIdx=" << targetIdx << std::endl;

	mFilterDevice->setTargetPos(targetIdx);

	startAnimation();

	m_ui->cbxSelectedFilter->setEnabled(false);

	// TODO: Write status text...
	//m_ui->lblStatus->setText(QString::fromStdString(std::to_string(mFilterDevice->getTargetPos())));
}

//void FilterCntlPanelT::onFilterAbortRequest() {
//	LOG(debug) << "FilterCntlPanelT::onFilterAbortRequest..." << std::endl;
//	mFilterDevice->abortMotion();
//}
