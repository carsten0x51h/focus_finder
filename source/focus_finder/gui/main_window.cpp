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

// TODO: Remove mFfl... - replace by static call...
// TODO: std::optional should not be passed as function parameter!

#include <QMessageBox>
#include <QToolBar>
#include <QIcon>
#include <QImage>
#include <QSpacerItem>
#include <QResizeEvent>
#include <QDockWidget>
#include <QGridLayout>

#include <memory>
#include <iomanip>
#include <utility>

#include "include/anim_menu_button.h"

#include "include/focus_curve_recorder_dialog.h"
#include "include/focus_curve_view_panel.h"
#include "include/image_viewer_panel.h"
#include "include/focus_cntl_panel.h"
#include "include/filter_cntl_panel.h"
#include "include/hfd_view_panel.h"
#include "include/fwhm_view_panel.h"
#include "include/reporting_viewer_panel.h"
#include "include/image_converter_panel.h"

#include "include/about_dialog.h"

#include "include/main_window.h"

#include "include/device_manager_dialog_factory.h"
#include "include/manage_device_profiles_dialog.h"

#include "../common/include/focus_finder.h"
#include "../common/include/profile_manager.h"
#include "../common/include/image_converter_16to8.h"

#include "ui_main_window.h"

Q_DECLARE_METATYPE(std::chrono::milliseconds)

Q_DECLARE_METATYPE(std::shared_ptr<FocusCurveRecordT>)

Q_DECLARE_METATYPE(std::shared_ptr<const FocusCurveRecordSetContainerT>)

Q_DECLARE_METATYPE(std::optional<FocusFinderProfileT>)

using namespace std::chrono_literals;

std::shared_ptr<CameraInterfaceT> MainWindow::getCurrentCamera() {
    return FocusFinderLogicT::get()->getCurrentCamera();
}

std::shared_ptr<FocusInterfaceT> MainWindow::getCurrentFocus() {
    return FocusFinderLogicT::get()->getCurrentFocus();
}

std::shared_ptr<FilterInterfaceT> MainWindow::getCurrentFilter() {
    return FocusFinderLogicT::get()->getCurrentFilter();
}

void MainWindow::onManageDeviceProfiles() {
    LOG(debug)
        << "MainWindow::onManageDeviceProfiles" << std::endl;

    mManageDeviceProfilesDialog->show();
}

void MainWindow::setBtnIcon(QAbstractButton *btn,
                            const std::string &filename) {
    QIcon icon;

    icon.addFile(QString::fromStdString(filename), QSize(), QIcon::Normal,
                 QIcon::Off);

    btn->setIcon(icon);
    btn->setMinimumSize(QSize(48, 48));
    btn->setIconSize(QSize(48, 48));
}

void MainWindow::setStatusIcon(QLabel *btn, const std::string &filename,
                               const std::string &toolTip) {
    QIcon icon;
    icon.addFile(QString::fromStdString(filename), QSize(), QIcon::Normal,
                 QIcon::Off);

    btn->setPixmap(icon.pixmap(QSize(16, 16)));
    btn->setToolTip(QString::fromStdString(toolTip));
}

void MainWindow::onSelectSubFramePressed() {
    LOG(debug)
        << "MainWindow::onSelectSubFramePressed... checked: "
        << mSelectSubFrameButton->isChecked() << std::endl;

    // Set "SELECT_RIO" mode in image viewer...
    if (mSelectSubFrameButton->isChecked()) {
        LOG(debug)
            << "Setting image viewer back to NAV..." << std::endl;
        mImageViewerPanel->setMode(ImageViewerModeT::NAVIGATION);

        // Change the icon back
        setBtnIcon(mSelectSubFrameButton, ":/res/subframe_64x64.png");

        // Clear the selection
        mImageViewerPanel->clearSelection();
    } else {
        LOG(debug)
            << "Setting image viewer to RIO_SELECT..." << std::endl;
        mImageViewerPanel->setMode(ImageViewerModeT::ROI_SELECT);
    }

    updateFocusFinderMainMenuBar();
}

void MainWindow::onStartFocusFinderPressed() {
    // TODO: Maybe move parts of this function to Ffl...
    LOG(debug)
        << "MainWindow::onStartFocusFinderPressed... checked: "
        << mStartFocusFinderButton->isChecked() << std::endl;

    auto ffExec = mFfl.getFocusFinderExecutor();
    bool isActive = mStartFocusFinderButton->isChecked();

    if (!isActive) {

        // TODO: Check that ffExec it is not yet running?

        // TODO: Check that every device is connected?

        // TODO: Always create a new focus finder instance? Otherwise.. where to store?
        //       -> depends on selection... -> FocusFinder profile...

        auto focusController = std::make_shared<FocusControllerT>(mFfl.getCurrentCamera(), mFfl.getCurrentFocus(),
                                                                  mFfl.getCurrentFilter());

        auto lastFocusStarPosOpt = mFfl.getLastFocusStarPos();

        //if (!lastFocusStarPosOpt) {
        // TODO: Handle case where no focus star is set / available
        //}
        focusController->setLastFocusStarPos(lastFocusStarPosOpt.value());


        auto activeProfileOpt = mFfl.getProfileManager()->getActiveProfile();

        // TODO: Make sure that activeProfile is set...
        //		if (!activeProfileOpt) {
        //		}
        focusController->setFocusFinderProfile(activeProfileOpt.value());



        // HACK / TODO: For now we create always a new instance...
        // TODO: Do not create here?! -> Factory?
        // TODO: Do not hardcode AVERAGED_FOCUS_CURVE.... get from profile...
        //auto focusFinder = FocusFinderFactoryT::getInstance(
        //        FocusFinderStrategyT::AVERAGED_FOCUS_CURVE, focusController);
        auto focusFinder = FocusFinderFactoryT::getInstance(
                FocusFinderStrategyT::SINGLE_PASS, focusController);

        // Register focusFinder UI listeners
        // TODO: Is unregister actually required? We create a new instalce always...
        // FoFi started
        focusFinder->registerFocusFinderStartedListener([&]() {
            emit focusFinderStartedSignal();
        });

        qRegisterMetaType<std::shared_ptr<FocusCurveRecordT> >("FocusCurveRecordPtrT");

        // HACK: Move up!?
        qRegisterMetaType<std::shared_ptr<FocusCurveRecordSetT> >("FocusCurveRecordSetPtrT");
        // TODO / FIXME: For some reason the following line is required HERE - it is not sufficient to use the MACRO above. Furthermore it is not sufficient to register this type in the focus_curve_recorder_panel...
        qRegisterMetaType<std::shared_ptr<const FocusCurveRecordSetContainerT> >("FocusCurveRecordSetContainerPtrT");


        // FoFi running / status update
        focusFinder->registerFocusFinderProgressUpdateListener(
                [&](float progress, const std::string &msg, std::shared_ptr<FocusCurveRecordT> record) {
                    emit focusFinderProgressUpdateSignal(progress, QString::fromStdString(msg), std::move(record));
                });

        // FoFi finished
        focusFinder->registerFocusFinderFinishedListener([&]() {
            emit focusFinderFinishedSignal();
        });

        // FoFi cancel
        focusFinder->registerFocusFinderCancelledListener([&]() {
            emit focusFinderCancelledSignal();
        });

        ffExec->setTask(focusFinder);

        LOG(debug)
            << "Starting '" << focusFinder->getName() << "'..." << std::endl;

        ffExec->start();
    } else {
        LOG(info)
            << "Cancelling running focus finder..." << std::endl;
        ffExec->cancel();

        mStartFocusFinderButton->setEnabled(false);

        LOG(info)
            << "Running focus finder cancelled." << std::endl;
    }

    updateFocusFinderMainMenuBar();
}

void MainWindow::createManageDeviceProfilesButton() {

    mManageDeviceProfilesButton = new QPushButton;

    setBtnIcon(mManageDeviceProfilesButton,
               ":/res/manage_device_profiles_64x64.png");

    // Define actions
    connect(mManageDeviceProfilesButton, &QPushButton::pressed, this,
            &MainWindow::onManageDeviceProfiles);

    getFocusFinderMainMenuBar()->addWidget(mManageDeviceProfilesButton);
}

/////////////////////////////////////////////////////////////////////////
// Image Converter Panel
/////////////////////////////////////////////////////////////////////////
void MainWindow::createImageConverterPanel() {
    auto *dock = new QDockWidget(tr("Brightness curve"), this); // TODO: Cleanup?
    dock->setAllowedAreas(Qt::RightDockWidgetArea);

    mImageConverterPanel = new ImageConverterPanelT(dock, mFfl);

    // See https://www.qtcentre.org/threads/64634-Add-QWidget-to-QDockWidget
    dock->setWidget(mImageConverterPanel);

    connect(mImageConverterPanel, &ImageConverterPanelT::valueChangedSignal,
            this, &MainWindow::onImageConverterValueChanged);

    dock->setMinimumSize(mImageConverterPanel->minimumSize());

    addDockWidget(Qt::RightDockWidgetArea, dock);
    m_ui->viewMenu->addAction(dock->toggleViewAction());

    // TODO: REMOVE - JUST FOR TESTING MULTIPLE DOCKS
//	QDockWidget *dock2 = new QDockWidget(tr("Brightness curve2"), this); // TODO: Cleanup
//	dock2->setAllowedAreas(Qt::RightDockWidgetArea); // Qt::LeftDockWidgetArea
//
//	mImageConverterPanel = new ImageConverterPanelT(dock2, mFfl);
//
//    dock2->setMinimumSize(mImageConverterPanel->minimumSize());
//
//	//dock->setWidget(customerList);
//	//m_ui->layImageConverterArea
//
//	addDockWidget(Qt::RightDockWidgetArea, dock2);
//	//viewMenu->addAction(dock->toggleViewAction()); // TODO: Add menu entry
}

void MainWindow::createReportingViewerPanel() {
    auto *dock = new QDockWidget(tr("Reporting"), this);
    dock->setAllowedAreas(Qt::BottomDockWidgetArea);

    mReportingViewerPanel = new ReportingViewerPanelT(dock);

    // See https://www.qtcentre.org/threads/64634-Add-QWidget-to-QDockWidget
    dock->setWidget(mReportingViewerPanel);

    dock->setMinimumSize(mReportingViewerPanel->minimumSize());
    addDockWidget(Qt::BottomDockWidgetArea, dock);
    m_ui->viewMenu->addAction(dock->toggleViewAction());
}

void MainWindow::createFocusCntlPanel() {
    auto *dock = new QDockWidget(tr("Focus control"), this); // TODO: Cleanup
    dock->setAllowedAreas(Qt::LeftDockWidgetArea);

    mFocusCntlPanel = new FocusCntlPanelT(dock, mFfl);

    // See https://www.qtcentre.org/threads/64634-Add-QWidget-to-QDockWidget
    dock->setWidget(mFocusCntlPanel);

    dock->setMinimumSize(mFocusCntlPanel->minimumSize());

    addDockWidget(Qt::LeftDockWidgetArea, dock);
    m_ui->viewMenu->addAction(dock->toggleViewAction());
}

void MainWindow::createFilterCntlPanel() {
    auto *dock = new QDockWidget(tr("Filter control"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea);

    mFilterCntlPanel = new FilterCntlPanelT(dock, mFfl);

    // See https://www.qtcentre.org/threads/64634-Add-QWidget-to-QDockWidget
    dock->setWidget(mFilterCntlPanel);

    dock->setMinimumSize(mFilterCntlPanel->minimumSize());

    addDockWidget(Qt::LeftDockWidgetArea, dock);
    m_ui->viewMenu->addAction(dock->toggleViewAction());
}

void MainWindow::createHfdViewPanel() {
    auto *dock = new QDockWidget(tr("HFD View"), this);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);

    mHfdViewPanel = new HfdViewPanelT(dock);

    // See https://www.qtcentre.org/threads/64634-Add-QWidget-to-QDockWidget
    dock->setWidget(mHfdViewPanel);

    dock->setMinimumSize(mHfdViewPanel->minimumSize());

    addDockWidget(Qt::RightDockWidgetArea, dock);
    m_ui->viewMenu->addAction(dock->toggleViewAction());
}

void MainWindow::createFwhmViewPanels() {
    {
        auto *dock = new QDockWidget(tr("FWHM(Horz) View"), this);
        dock->setAllowedAreas(Qt::RightDockWidgetArea);

        mFwhmHorzViewPanel = new FwhmViewPanelT(dock, mFfl);

        // See https://www.qtcentre.org/threads/64634-Add-QWidget-to-QDockWidget
        dock->setWidget(mFwhmHorzViewPanel);

        dock->setMinimumSize(mFwhmHorzViewPanel->minimumSize());

        addDockWidget(Qt::RightDockWidgetArea, dock);
        m_ui->viewMenu->addAction(dock->toggleViewAction());
    }

    {
        auto *dock = new QDockWidget(tr("FWHM(Vert) View"), this);
        dock->setAllowedAreas(Qt::RightDockWidgetArea);

        mFwhmVertViewPanel = new FwhmViewPanelT(dock, mFfl);

        // See https://www.qtcentre.org/threads/64634-Add-QWidget-to-QDockWidget
        dock->setWidget(mFwhmVertViewPanel);

        dock->setMinimumSize(mFwhmVertViewPanel->minimumSize());

        addDockWidget(Qt::RightDockWidgetArea, dock);
        m_ui->viewMenu->addAction(dock->toggleViewAction());
    }
}




void MainWindow::onImageConverterValueChanged() {
    LOG(debug)
        << "MainWindow::onImageConverterValueChanged..." << std::endl;
    updateImage();
}

/////////////////////////////////////////////////////////////////////////
// Status bar
/////////////////////////////////////////////////////////////////////////

void MainWindow::onCameraDeviceConnected() {
    LOG(debug)
        << "MainWindow::onCameraDeviceConnected..." << std::endl;
    updateCurrentCameraConnectionStateUI();
}

void MainWindow::onCameraDeviceConnecting() {
    LOG(debug)
        << "MainWindow::onCameraDeviceConnecting..." << std::endl;
    updateCurrentCameraConnectionStateUI();
}

void MainWindow::onCameraDeviceDisconnected() {
    updateCurrentCameraConnectionStateUI();
}

void MainWindow::onFocusDeviceConnected() {
    updateCurrentFocusConnectionStateUI();
}

void MainWindow::onFocusDeviceConnecting() {
    updateCurrentFocusConnectionStateUI();
}

void MainWindow::onFocusDeviceDisconnected() {
    updateCurrentFocusConnectionStateUI();
}

void MainWindow::onFilterDeviceConnected() {
    updateCurrentFilterConnectionStateUI();
}

void MainWindow::onFilterDeviceConnecting() {
    updateCurrentFilterConnectionStateUI();
}

void MainWindow::onFilterDeviceDisconnected() {
    updateCurrentFilterConnectionStateUI();
}

void MainWindow::createStatusBar() {

    // Connect "Camera" status change to "LED"
    connect(this, &MainWindow::cameraDeviceConnectedSignal, this,
            &MainWindow::onCameraDeviceConnected);

    connect(this, &MainWindow::cameraDeviceConnectingSignal, this,
            &MainWindow::onCameraDeviceConnecting);

    connect(this, &MainWindow::cameraDeviceDisconnectedSignal, this,
            &MainWindow::onCameraDeviceDisconnected);

    // TODO: Get initial camera status...
    mCameraConnectionStatusLabel = new QLabel;

    updateCurrentCameraConnectionStateUI();

    // Add icon to status bar
    // https://stackoverflow.com/questions/11986280/add-icon-to-status-bar-qt
    statusBar()->addPermanentWidget(mCameraConnectionStatusLabel); // Align right

    // TODO: Create sub function for each edevice? Or even generic function?

    // Connect "Focus" status change to "LED"
    connect(this, &MainWindow::focusDeviceConnectedSignal, this,
            &MainWindow::onFocusDeviceConnected);

    connect(this, &MainWindow::focusDeviceConnectingSignal, this,
            &MainWindow::onFocusDeviceConnecting);

    connect(this, &MainWindow::focusDeviceDisconnectedSignal, this,
            &MainWindow::onFocusDeviceDisconnected);

    // TODO: Get initial Focus status...
    mFocusConnectionStatusLabel = new QLabel;

    updateCurrentFocusConnectionStateUI();

    statusBar()->addPermanentWidget(mFocusConnectionStatusLabel); // Align right

    // Connect "Filter" status change to "LED"
    connect(this, &MainWindow::filterDeviceConnectedSignal, this,
            &MainWindow::onFilterDeviceConnected);

    connect(this, &MainWindow::filterDeviceConnectingSignal, this,
            &MainWindow::onFilterDeviceConnecting);

    connect(this, &MainWindow::filterDeviceDisconnectedSignal, this,
            &MainWindow::onFilterDeviceDisconnected);

    // TODO: Get initial Focus status...
    mFilterConnectionStatusLabel = new QLabel;

    updateCurrentFilterConnectionStateUI();

    statusBar()->addPermanentWidget(mFilterConnectionStatusLabel); // Align right
}

/////////////////////////////////////////////////////////////////////////
// Exposure button
/////////////////////////////////////////////////////////////////////////
void MainWindow::onExposurePressed() {

    auto activeCamera = mFfl.getCurrentCamera();

    if (!activeCamera) {
        LOG(warning)
            << "No camera set!" << std::endl;
        return;
    }

    LOG(debug)
        << "Start exposure... isExposureRunning=" << activeCamera->isExposureRunning() << std::endl;

    if (mExposureButton->isChecked()) {
        LOG(debug)
            << "Button already pressed.... -> cancel exposure if running."
            << std::endl;
        activeCamera->cancelExposure();
        mExposureButton->setEnabled(false);
    } else {
        LOG(debug)
            << "Start exposure..." << std::endl;

        updateExposureTime();

        // Extract exposure time
        mExposureButton->startAnimation();

        activeCamera->startExposure();
    }

    updateFocusFinderMainMenuBar();
}

void MainWindow::onFrameTransferUpd(double percentage) {
    LOG(debug)
        << "onFrameTransferUpd - percentage: " << percentage << std::endl;
}

void MainWindow::onExposureLoopPressed() {
    auto activeCamera = mFfl.getCurrentCamera();

    if (!activeCamera) {
        LOG(warning)
            << "No camera set!" << std::endl;
        return;
    }

    LOG(debug)
        << "onExposureLoopPressed - checked: " << mExposureLoopAction->isChecked()
        << std::endl;

    LoopModeT::TypeE loopMode = (
            mExposureLoopAction->isChecked() ?
            LoopModeT::LOOP : LoopModeT::SINGLE);

    activeCamera->setLoopMode(loopMode);
}

void MainWindow::onExposureCycleFinished(bool lastExposure) {
    LOG(debug)
        << "onExposureCycleFinished - lastExposure: " << lastExposure << std::endl;

    updateImage();

    if (lastExposure) {
        // Disable button
        setExposureButtonState(false /*exposure not running*/);
    }

    updateFocusFinderMainMenuBar();

    // Update status bar
    statusBar()->showMessage("Exposure finished.");
}

void MainWindow::setExposureButtonState(bool isRunning) {
    LOG(debug)
        << "MainWindow::setExposureButtonState...isRunning=" << isRunning
        << std::endl;

    if (isRunning) {
        mExposureButton->startAnimation();
        mExposureButton->setEnabled(true);
        mExposureButton->setChecked(true);
    } else {
        mExposureButton->stopAnimation();
        mExposureButton->setEnabled(true);
        mExposureButton->setChecked(false);
        setBtnIcon(mExposureButton, ":/res/aperture_64x64.png");
    }

    // Set loop mode (is part of the exposure button...)
    if (getCurrentCamera()) {
        mExposureLoopAction->setChecked(
                getCurrentCamera()->getLoopMode() == LoopModeT::LOOP);
    }
}

void MainWindow::onExposureCancelled() {
    LOG(debug)
        << "MainWindow::onExposureCancelled..." << std::endl;

    setExposureButtonState(false /*exposure not running*/);
    updateFocusFinderMainMenuBar();

    statusBar()->showMessage("Exposure cancelled.");

    // NOTE: setChecked(false) does not work for some reason...
    mExposureButton->setEnabled(false);
    mExposureButton->setChecked(false);
    mExposureButton->setEnabled(true);
    //mExposureButton->toggle();
}

void MainWindow::onExposureTimerUpd(double t) {
    std::stringstream ss;
    ss << "Exposure running..." << std::fixed << std::setprecision(1) << t
       << "s" << std::endl;
    statusBar()->showMessage(QString::fromStdString(ss.str()));
}

void MainWindow::onExposureDelayTimerUpd(double t) {
    std::stringstream ss;
    ss << "Exposure starts in..." << std::fixed << std::setprecision(1) << t
       << "s" << std::endl;
    statusBar()->showMessage(QString::fromStdString(ss.str()));
}

void MainWindow::updateImage() {
    const FrameT &lastFrame = mFfl.getLastFrame();

    if (!lastFrame.isEmpty()) {
        std::shared_ptr<const ImageT> imgPtr = lastFrame.getImage();
        RectT<unsigned int> roiRect = lastFrame.getRoi();

        LOG(debug)
            << "MainWindow::updateImage... roiRect.isSet()? " << roiRect.isSet()
            << std::endl;

        cimg_library::CImg<unsigned char> displayImg;

        // TODO: Apply move semantics and return result instead?
        // The image converter holds the mapping of values in a LUT for a fast image
        // conversion. This is why it needs to be instantiated and a static function
        // is not sufficient.
        mFfl.getImageConverter16To8()->convert(*imgPtr /*src*/,
                                               &displayImg /*dest*/);

        // Finally, put the result into an QImage.
        QImage qtImage(displayImg.width(), displayImg.height(),
                       QImage::Format_ARGB32);

        cimg_forXY(displayImg, x, y) {
                int pixValue = displayImg(x, y,
                                          0); // TODO: convert function must handle case of RGB input and mono -> depth() == 1?
                qtImage.setPixel(x, y, qRgb(pixValue, pixValue, pixValue));
            }

        // Set image
        if (roiRect.isSet()) {
            LOG(debug)
                << "MainWindow::updateImage()...roiRect.isSet() - setSubFrame..."
                << std::endl;

            // Convert sub frame rect to QRect for UI usage
            QRect qtRoiRect(roiRect.x(), roiRect.y(), roiRect.width(),
                            roiRect.height());

            mImageViewerPanel->setSubFrame(qtRoiRect, qtImage);
        } else {
            LOG(debug)
                << "MainWindow::updateImage()...roiRect.isSet() - setFrame..."
                << std::endl;

            mImageViewerPanel->setFrame(qtImage);
        }
    }
}

void MainWindow::createExposureButton() {
    mExposureButton = new AnimMenuButtonT;

    mExposureButton->setPopupMode(QToolButton::MenuButtonPopup);

    mExposureLoopAction = new QAction("Loop...", this);
    mExposureLoopAction->setStatusTip(tr("Loop exposure"));
    mExposureLoopAction->setCheckable(true);

    setBtnIcon(mExposureButton, ":/res/aperture_64x64.png");

    connect(mExposureButton, &QToolButton::pressed, this,
            &MainWindow::onExposurePressed);

    // Add "Loop" menu
    mExposureButtonMenu = new QMenu;
    mExposureButtonMenu->addAction(mExposureLoopAction);

    mExposureButton->setMenu(mExposureButtonMenu);
    mExposureButton->setCheckable(true);

    setExposureButtonState(false);

    connect(mExposureLoopAction, &QAction::triggered, this,
            &MainWindow::onExposureLoopPressed);

    // Connect camera events with exposure button
    connect(this, &MainWindow::exposureCycleFinishedSignal, this,
            &MainWindow::onExposureCycleFinished);

    connect(this, &MainWindow::exposureCancelledSignal, this,
            &MainWindow::onExposureCancelled);

    connect(this, &MainWindow::frameTransferUpdSignal, this,
            &MainWindow::onFrameTransferUpd);

    connect(this, &MainWindow::exposureTimerUpdSignal, this,
            &MainWindow::onExposureTimerUpd);

    connect(this, &MainWindow::exposureDelayTimerUpdSignal, this,
            &MainWindow::onExposureDelayTimerUpd);

    getFocusFinderMainMenuBar()->addWidget(mExposureButton);
}

void MainWindow::updateExposureTime() {

    auto currentCamera = getCurrentCamera();

    if (currentCamera) {
        QVariant data = mExposureTimeCbx->currentData();

        auto t = data.value<std::chrono::milliseconds>();

        LOG(debug)
            << "MainWindow::updateExposureTime - t=" << t.count() << "ms"
            << std::endl;

        currentCamera->setExposureTime(t);


        // Also store the respective value in the profile
        auto activeProfileOpt = mFfl.getProfileManager()->getActiveProfile();
        auto seconds = std::chrono::duration_cast<std::chrono::duration<float> >(t);

        LOG(debug)
            << "MainWindow::updateExposureTime - storing value t=" << seconds.count() << "s to profile..."
            << std::endl;

        FocusFinderProfileT activeProfile = activeProfileOpt.value();
        activeProfile.setExposureTime(seconds);
        mFfl.getProfileManager()->updateProfile(
                mFfl.getProfileManager()->getActiveProfileDirectoryName() /*profileDirectoryName*/, activeProfile);
    }
}

void MainWindow::onExposureTimeSelectionChangedSlot() {
    LOG(debug)
        << "MainWindow::onExposureTimeSelectionChangedSlot..." << std::endl;
    updateExposureTime();
}

void MainWindow::updateExposureTimeSelector() {
    // NOTE: This log pollutes the logfile...
    // LOG(debug)
    // << "MainWindow::updateExposureTimeSelector..." << std::endl;

    auto currentCamera = getCurrentCamera();


    mExposureTimeCbx->blockSignals(true);

    mExposureTimeCbx->clear();


    auto activeProfile = mFfl.getProfileManager()->getActiveProfile();
    bool focusFinderRunning = mFfl.getFocusFinderExecutor()->isRunning();

    bool hasActiveProfile = activeProfile.has_value();
    bool cameraSelected = (currentCamera != nullptr);
    bool cameraConnected =
            currentCamera && currentCamera->getParentDevice()->isConnected();

    bool exposureRunning = currentCamera && currentCamera->isExposureRunning();
    bool enableExposureTimeSelector =
            hasActiveProfile && cameraSelected && cameraConnected && !exposureRunning && !focusFinderRunning;

    mExposureTimeCbx->setEnabled(enableExposureTimeSelector);


    // Fill in exposure time values. Those depend on the abilities of the camera.
    // Get min possible exposure time from camera. Use this value to limit the values
    // in the cbx.
    if (currentCamera && currentCamera->getParentDevice()->isConnected()) {

        std::vector<std::chrono::milliseconds> exposureTimes{10ms, 50ms,
                                                             100ms, 500ms, 1000ms, 1500ms, 2000ms, 3000ms, 4000ms,
                                                             5000ms,
                                                             6000ms, 7000ms, 8000ms, 9000ms, 10000ms};

        std::chrono::milliseconds minExposureTimeMillisecs;

        try {
            minExposureTimeMillisecs = currentCamera->getMinExposureTime();
        } catch (CameraExceptionT &exc) {
            minExposureTimeMillisecs = exposureTimes.at(0);
        }

        for (auto const &t : exposureTimes) {
            if (t >= minExposureTimeMillisecs) {

                auto seconds = std::chrono::duration_cast<
                        std::chrono::duration<float> >(t);

                std::stringstream stream;
                stream << std::fixed << std::setprecision(2) << seconds.count()
                       << "s";

                QVariant data;
                data.setValue(t);

                mExposureTimeCbx->addItem(QString::fromStdString(stream.str()),
                                          data);
            }
        }


        // Set selected entry
        // TODO: This is not so nice...
        //auto cameraExposureTime = mCameraDevice->getExposureTime();
        auto cameraExposureTime = activeProfile.value().getExposureTime();

        auto seconds =
                std::chrono::duration_cast<std::chrono::duration<float> >(
                        cameraExposureTime);

        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << seconds.count() << "s";

        int idx = mExposureTimeCbx->findText(
                QString::fromStdString(stream.str()));
        mExposureTimeCbx->setCurrentIndex(idx);

        mExposureTimeCbx->blockSignals(false);

    } else {
        mExposureTimeCbx->setEnabled(false);
    }
}

void MainWindow::createExposureTimeSelector() {
    mExposureTimeCbx = new QComboBox;
    mExposureTimeCbx->setMinimumSize(QSize(48, 30));

    // Connect cbx selection listener...
    connect(mExposureTimeCbx,
            QOverload<const QString &>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onExposureTimeSelectionChangedSlot);

    getFocusFinderMainMenuBar()->addWidget(mExposureTimeCbx);
}

void MainWindow::updateCurrentCameraConnectionStateUI() {
    auto currentCamera = getCurrentCamera();

    if (currentCamera) {
        auto connState = currentCamera->getParentDevice()->getConnectionState();
        switch (connState) {
            case DeviceConnectionStateT::CONNECTED: {
                setStatusIcon(mCameraConnectionStatusLabel,
                              ":/res/green_dot_64x64.png", "Camera connected");

                updateFocusFinderMainMenuBar();
                break;
            }
            case DeviceConnectionStateT::CONNECTING: {
                setStatusIcon(mCameraConnectionStatusLabel,
                              ":/res/yellow_dot_64x64.png", "Camera connecting");

                updateFocusFinderMainMenuBar();
                break;
            }
            case DeviceConnectionStateT::DISCONNECTED:
            default:
                setStatusIcon(mCameraConnectionStatusLabel,
                              ":/res/red_dot_64x64.png", "Camera disconnected");

                // Update the UI - get exposure status from new camera and set the exposure button accordingly
                if (currentCamera) {
                    setExposureButtonState(currentCamera->isExposureRunning());
                }

                updateFocusFinderMainMenuBar();
                break;
        }
    } else {
        setStatusIcon(mCameraConnectionStatusLabel, ":/res/red_dot_64x64.png",
                      "Camera disconnected");

        // Update the UI - get exposure status from new camera and set the exposure button accordingly
        if (currentCamera) {
            setExposureButtonState(currentCamera->isExposureRunning());
        }

        updateFocusFinderMainMenuBar();
    }
}

void MainWindow::updateCameraDevice(const std::shared_ptr<CameraInterfaceT>& oldCameraInterface,
                                    const std::shared_ptr<CameraInterfaceT>& newCameraInterface) {

    if (oldCameraInterface == newCameraInterface) {
        return;
    }

    bool hasOldCameraInterface = (oldCameraInterface != nullptr);

    if (hasOldCameraInterface) {
        auto oldCameraDevice = oldCameraInterface->getParentDevice();

        LOG(debug)
            << "MainWindow::updateCameraDevice... unregistering from old camera..."
            << oldCameraDevice->getName() << std::endl;

        // There was already an old device - unregister listener and register to the new one.
        oldCameraDevice->unregisterDeviceConnectedListener(
                mCameraDeviceConnectedConnection);
        oldCameraDevice->unregisterDeviceConnectingListener(
                mCameraDeviceConnectingConnection);
        oldCameraDevice->unregisterDeviceDisconnectedListener(
                mCameraDeviceDisconnectedConnection);

        // There was already an old device interface - unregister listener and register to the new one.
        oldCameraInterface->unregisterExposureCycleFinishedListener(
                mExposureCycleFinishedConnection);
        oldCameraInterface->unregisterExposureCancelledListener(
                mExposureCancelledConnection);
        oldCameraInterface->unregisterFrameTransferUpdListener(
                mFrameTransferUpdConnection);
        oldCameraInterface->unregisterExposureTimerUpdListener(
                mFrameExposureTimerUpdConnection);
        oldCameraInterface->unregisterExposureDelayTimerUpdListener(
                mFrameExposureDelayTimerUpdConnection);
    }

    // Register to new device
    if (newCameraInterface) {
        auto newCameraDevice = newCameraInterface->getParentDevice();

        LOG(debug)
            << "MainWindow::updateCameraDevice... registering to new camera..."
            << newCameraDevice->getName() << std::endl;

        mCameraDeviceConnectedConnection =
                newCameraDevice->registerDeviceConnectedListener(
                        [&]() { emit cameraDeviceConnectedSignal(); });
        mCameraDeviceConnectingConnection =
                newCameraDevice->registerDeviceConnectingListener(
                        [&]() { emit cameraDeviceConnectingSignal(); });
        mCameraDeviceDisconnectedConnection =
                newCameraDevice->registerDeviceDisconnectedListener(
                        [&]() { emit cameraDeviceDisconnectedSignal(); });

        mExposureCycleFinishedConnection =
                newCameraInterface->registerExposureCycleFinishedListener(
                        [&](RectT<unsigned int> /*roiRect*/, const std::shared_ptr<const ImageT>& /*resultImage*/,
                            bool lastExposure) {
                            emit exposureCycleFinishedSignal(lastExposure);
                        });
        mExposureCancelledConnection =
                newCameraInterface->registerExposureCancelledListener(
                        [&]() { emit exposureCancelledSignal(); });
        mFrameTransferUpdConnection =
                newCameraInterface->registerFrameTransferUpdListener(
                        [&](double percentage) { emit frameTransferUpdSignal(percentage); });
        mFrameExposureTimerUpdConnection =
                newCameraInterface->registerExposureTimerUpdListener(
                        [&](double t) { emit exposureTimerUpdSignal(t); });
        mFrameExposureDelayTimerUpdConnection =
                newCameraInterface->registerExposureDelayTimerUpdListener(
                        [&](double t) { emit exposureDelayTimerUpdSignal(t); });
    }

    updateCurrentCameraConnectionStateUI();
}

void MainWindow::updateCurrentFocusConnectionStateUI() {
    auto currentFocus = getCurrentFocus();

    if (currentFocus) {
        auto connState = currentFocus->getParentDevice()->getConnectionState();
        switch (connState) {
            case DeviceConnectionStateT::CONNECTED: {
                setStatusIcon(mFocusConnectionStatusLabel,
                              ":/res/green_dot_64x64.png", "Focus connected");

                updateFocusFinderMainMenuBar();
                break;
            }
            case DeviceConnectionStateT::CONNECTING: {
                setStatusIcon(mFocusConnectionStatusLabel,
                              ":/res/yellow_dot_64x64.png", "Focus connecting");

                updateFocusFinderMainMenuBar();
                break;
            }
            default:
            case DeviceConnectionStateT::DISCONNECTED: {
                setStatusIcon(mFocusConnectionStatusLabel,
                              ":/res/red_dot_64x64.png", "Focus disconnected");

                updateFocusFinderMainMenuBar();
                break;
            }
        }
    } else {
        setStatusIcon(mFocusConnectionStatusLabel, ":/res/red_dot_64x64.png",
                      "Focus disconnected");

        updateFocusFinderMainMenuBar();
    }
}

void MainWindow::updateCurrentFilterConnectionStateUI() {
    auto currentFilter = getCurrentFilter();

    if (currentFilter) {
        auto connState = currentFilter->getParentDevice()->getConnectionState();
        switch (connState) {
            case DeviceConnectionStateT::CONNECTED: {
                setStatusIcon(mFilterConnectionStatusLabel,
                              ":/res/green_dot_64x64.png", "Filter connected");

                updateFocusFinderMainMenuBar();
                break;
            }
            case DeviceConnectionStateT::CONNECTING: {
                setStatusIcon(mFilterConnectionStatusLabel,
                              ":/res/yellow_dot_64x64.png", "Filter connecting");

                updateFocusFinderMainMenuBar();
                break;
            }
            case DeviceConnectionStateT::DISCONNECTED:
            default:
                setStatusIcon(mFilterConnectionStatusLabel,
                              ":/res/red_dot_64x64.png", "Filter disconnected");

                updateFocusFinderMainMenuBar();
                break;
        }
    } else {
        setStatusIcon(mFilterConnectionStatusLabel, ":/res/red_dot_64x64.png",
                      "Filter disconnected");

        updateFocusFinderMainMenuBar();
    }
}

void MainWindow::updateFocusDevice(const std::shared_ptr<FocusInterfaceT>& oldFocusInterface,
                                   const std::shared_ptr<FocusInterfaceT>& newFocusInterface) {

    if (oldFocusInterface == newFocusInterface) {
        return;
    }

    bool hasOldFocusInterface = (oldFocusInterface != nullptr);

    if (hasOldFocusInterface) {
        auto oldFocusDevice = oldFocusInterface->getParentDevice();

        LOG(debug)
            << "MainWindow::updateFocusDevice... unregistering from old Focus..."
            << oldFocusDevice->getName() << std::endl;

        // There was already an old device - unregister listener and register to the new one.
        oldFocusDevice->unregisterDeviceConnectedListener(
                mFocusDeviceConnectedConnection);
        oldFocusDevice->unregisterDeviceConnectingListener(
                mFocusDeviceConnectedConnection);
        oldFocusDevice->unregisterDeviceDisconnectedListener(
                mFocusDeviceConnectedConnection);
    }

    // Register to new device
    if (newFocusInterface) {
        auto newFocusDevice = newFocusInterface->getParentDevice();

        LOG(debug)
            << "MainWindow::updateFocusDevice... registering to new Focus..."
            << newFocusDevice->getName() << std::endl;

        mFocusDeviceConnectedConnection =
                newFocusDevice->registerDeviceConnectedListener(
                        [&]() { emit focusDeviceConnectedSignal(); });
        mFocusDeviceConnectingConnection =
                newFocusDevice->registerDeviceConnectingListener(
                        [&]() { emit focusDeviceConnectingSignal(); });
        mFocusDeviceDisconnectedConnection =
                newFocusDevice->registerDeviceDisconnectedListener(
                        [&]() { emit focusDeviceDisconnectedSignal(); });
    }

    updateCurrentFocusConnectionStateUI();
}

void MainWindow::updateFilterDevice(const std::shared_ptr<FilterInterfaceT>& oldFilterInterface,
                                    const std::shared_ptr<FilterInterfaceT>& newFilterInterface) {
    if (oldFilterInterface == newFilterInterface) {
        return;
    }

    bool hasOldFilterInterface = (oldFilterInterface != nullptr);

    if (hasOldFilterInterface) {
        auto oldFilterDevice = oldFilterInterface->getParentDevice();

        LOG(debug)
            << "MainWindow::updateFilterDevice... unregistering from old Filter..."
            << oldFilterDevice->getName() << std::endl;

        // There was already an old device - unregister listener and register to the new one.
        oldFilterDevice->unregisterDeviceConnectedListener(
                mFilterDeviceConnectedConnection);
        oldFilterDevice->unregisterDeviceConnectingListener(
                mFilterDeviceConnectedConnection);
        oldFilterDevice->unregisterDeviceDisconnectedListener(
                mFilterDeviceConnectedConnection);
    }

    // Register to new device
    if (newFilterInterface) {
        auto * newFilterDevice = newFilterInterface->getParentDevice();

        LOG(debug)
            << "MainWindow::updateFilterDevice... registering to new Filter..."
            << newFilterDevice->getName() << std::endl;

        mFilterDeviceConnectedConnection =
                newFilterDevice->registerDeviceConnectedListener(
                        [&]() { emit filterDeviceConnectedSignal(); });
        mFilterDeviceConnectingConnection =
                newFilterDevice->registerDeviceConnectingListener(
                        [&]() { emit filterDeviceConnectingSignal(); });
        mFilterDeviceDisconnectedConnection =
                newFilterDevice->registerDeviceDisconnectedListener(
                        [&]() { emit filterDeviceDisconnectedSignal(); });
    }

    updateCurrentFilterConnectionStateUI();
}

void MainWindow::updateProfile(std::optional<FocusFinderProfileT> oldProfile,
                               std::optional<FocusFinderProfileT> newProfile) {

    auto deviceManager = FocusFinderLogicT::get()->getDeviceManager();

    // TODO: Factor out code ... - template and/or lambda function... or static_pointer_cast....
    auto oldCameraDevice = (oldProfile.has_value() ?
                            deviceManager->getDevice(oldProfile->getCameraDeviceName()) :
                            nullptr);

    auto oldCameraInterface = (oldCameraDevice != nullptr ? oldCameraDevice->getCameraInterface() : nullptr);

    auto newCameraDevice = (newProfile.has_value() ?
                            deviceManager->getDevice(newProfile->getCameraDeviceName()) :
                            nullptr);

    auto newCameraInterface = (newCameraDevice != nullptr ? newCameraDevice->getCameraInterface() : nullptr);


    updateCameraDevice(oldCameraInterface, newCameraInterface);


    auto oldFocusDevice = (oldProfile.has_value() ?
                           deviceManager->getDevice(oldProfile->getFocusDeviceName()) :
                           nullptr);

    auto oldFocusInterface = (oldFocusDevice != nullptr ? oldFocusDevice->getFocusInterface() : nullptr);

    auto newFocusDevice = (newProfile.has_value() ?
                           deviceManager->getDevice(newProfile->getFocusDeviceName()) :
                           nullptr);

    auto newFocusInterface = (newFocusDevice != nullptr ? newFocusDevice->getFocusInterface() : nullptr);

    updateFocusDevice(oldFocusInterface, newFocusInterface);


    auto oldFilterDevice = (oldProfile.has_value() ?
                            deviceManager->getDevice(oldProfile->getFilterDeviceName()) :
                            nullptr);

    auto oldFilterInterface = (oldFilterDevice != nullptr ? oldFilterDevice->getFilterInterface() : nullptr);

    auto newFilterDevice = (newProfile.has_value() ?
                            deviceManager->getDevice(newProfile->getFilterDeviceName()) :
                            nullptr);

    auto newFilterInterface = (newFilterDevice != nullptr ? newFilterDevice->getFilterInterface() : nullptr);

    updateFilterDevice(oldFilterInterface, newFilterInterface);
}

void MainWindow::createSelectSubFrameButton() {

    mSelectSubFrameButton = new AnimMenuButtonT;

    setBtnIcon(mSelectSubFrameButton, ":/res/subframe_64x64.png");

    mSelectSubFrameButton->setCheckable(true);
    mSelectSubFrameButton->setChecked(false);

    connect(mSelectSubFrameButton, &QToolButton::pressed, this,
            &MainWindow::onSelectSubFramePressed);

    getFocusFinderMainMenuBar()->addWidget(mSelectSubFrameButton);
}

void MainWindow::onFocusFinderStarted() {
    LOG(debug)
        << "MainWindow::onFocusFinderStarted..." << std::endl;

    mStartFocusFinderButton->startAnimation();
}

void MainWindow::onFocusFinderFinished() {
    LOG(debug)
        << "MainWindow::onFocusFinderFinished..." << std::endl;

    mStartFocusFinderButton->stopAnimation();

    // NOTE: setChecked(false) does not work for some reason...
    mStartFocusFinderButton->setEnabled(false);
    mStartFocusFinderButton->setChecked(false);
    mStartFocusFinderButton->setEnabled(true);

    updateFocusFinderMainMenuBar();
}

void MainWindow::onFocusFinderCancelled() {
    LOG(debug)
        << "MainWindow::onFocusFinderCancelled..." << std::endl;

    mStartFocusFinderButton->stopAnimation();

    // NOTE: setChecked(false) does not work for some reason...
    mStartFocusFinderButton->setEnabled(false);
    mStartFocusFinderButton->setChecked(false);
    mStartFocusFinderButton->setEnabled(true);

    updateFocusFinderMainMenuBar();
}

void MainWindow::onFocusFinderProgressUpdate(float progress,
                                             const QString &msg, const std::shared_ptr<FocusCurveRecordT>& record) {

    LOG(debug)
        << "MainWindow::onFocusFinderProgressUpdate(progress=" << progress
        << ", msg=" << msg.toStdString() << ", ...)..." << std::endl;

    if (record != nullptr) {
        LOG(debug) << "    -> record=" << *record << std::endl;

        mHfdViewPanel->setHfd(record->getHfd());
        mFwhmHorzViewPanel->setFwhm(record->getFwhmHorz());
        mFwhmVertViewPanel->setFwhm(record->getFwhmVert());

       // TODO:  mStarDetailsPanel->setStarDetails();

    }


    // TODO: Update mStartFocusFinderButton...
}

void MainWindow::onRecalibrationPressed() {
    LOG(debug) << "MainWindow::onRecalibrationPressed..." << std::endl;

    auto *focusCurveRecorderDialog = new FocusCurveRecorderDialogT(this,
                                                                                        mFfl.getFocusCurveRecorderLogic());
    focusCurveRecorderDialog->exec();

    delete focusCurveRecorderDialog; // TODO: Will probably be deleted when containerDialog is deleted...
}

void MainWindow::onEditCalibrationDataPressed() {
    LOG(debug) << "MainWindow::onEditCalibrationDataPressed..." << std::endl;
}

void MainWindow::createStartFocusFinderButton() {

    mStartFocusFinderButton = new AnimMenuButtonT;

    setBtnIcon(mStartFocusFinderButton, ":/res/find_focus_64x64.png");

    // Set popup menu mode
    mStartFocusFinderButton->setPopupMode(QToolButton::MenuButtonPopup);
    mRecalibrationAction = new QAction(this);
    mEditCalibrationDataAction = new QAction(this);
    mEditCalibrationDataAction->setText(QString::fromStdString("Edit calibration data..."));
    mEditCalibrationDataAction->setStatusTip(QString::fromStdString("Edit existing calibration data."));

    updateFocusFinderButtonMenuTexts();

    // Add "Calibration" menu
    mStartFocusFinderButtonMenu = new QMenu;
    mStartFocusFinderButtonMenu->addAction(mRecalibrationAction);
    mStartFocusFinderButtonMenu->addAction(mEditCalibrationDataAction);

    mStartFocusFinderButton->setMenu(mStartFocusFinderButtonMenu);
    mStartFocusFinderButton->setCheckable(true);
    mStartFocusFinderButton->setChecked(false);

    // Connect calibration menu items
    connect(mRecalibrationAction, &QAction::triggered, this,
            &MainWindow::onRecalibrationPressed);
    connect(mEditCalibrationDataAction, &QAction::triggered, this,
            &MainWindow::onEditCalibrationDataPressed);

    // Connect actual "focus finder" button
    connect(mStartFocusFinderButton, &QToolButton::pressed, this,
            &MainWindow::onStartFocusFinderPressed);

    // Connect focus finder executor events focus finder button
    connect(this, &MainWindow::focusFinderStartedSignal, this,
            &MainWindow::onFocusFinderStarted);

    connect(this, &MainWindow::focusFinderFinishedSignal, this,
            &MainWindow::onFocusFinderFinished);

    connect(this, &MainWindow::focusFinderCancelledSignal, this,
            &MainWindow::onFocusFinderCancelled);

    connect(this, &MainWindow::focusFinderProgressUpdateSignal, this,
            &MainWindow::onFocusFinderProgressUpdate);

    getFocusFinderMainMenuBar()->addWidget(mStartFocusFinderButton);
}

void MainWindow::onRoiClearedSlot() {
    LOG(debug)
        << "MainWindow::onRoiClearedSlot()..." << std::endl;
    mFfl.clearSelectedRoi();

    updateFocusFinderMainMenuBar();
}

void MainWindow::onRoiSelectedSlot(const QRect &roiRect,
                                   const QImage &roiImage) {
    LOG(debug)
        << "MainWindow::onRoiSelectedSlot()..." << std::endl;

    QRect normalizedRoiRect = roiRect.normalized();

    // TODO: Convert to isSet()...
    if (normalizedRoiRect.width() > 0 && normalizedRoiRect.height() > 0) {
        // Change icon on button
        setBtnIcon(mSelectSubFrameButton,
                   ":/res/selected_sub_frame_selected_64x64.png");

        mImageViewerPanel->setMode(ImageViewerModeT::NAVIGATION);

        RectT<unsigned int> selectedRoi(normalizedRoiRect.x(),
                                        normalizedRoiRect.y(), normalizedRoiRect.width(),
                                        normalizedRoiRect.height());
        mFfl.setSelectedRoi(selectedRoi);
    }

    // TODO: DO NOT USE THE ROI IMAGE - it will be removed! Instead use the roiRect to query the ffl to get the image or subframe as CImg...
    Q_UNUSED(roiImage)

    updateFocusFinderMainMenuBar();
}

void MainWindow::onPoiSelectedSlot(const QPoint &qtPoi) {
    LOG(debug)
        << "MainWindow::onPoiSelectedSlot()... x=" << qtPoi.x() << ", y="
        << qtPoi.y() << std::endl;

    PointT<float> poi(qtPoi.x(), qtPoi.y());

    auto focusStarPosOpt = mFfl.findFocusStar(poi);

    if (focusStarPosOpt) {
        PointT<float> center = *focusStarPosOpt;

        LOG(info)
            << "Star detected - center pos=(" << std::get<0>(center) << ", "
            << std::get<1>(center) << ")." << std::endl;

        // Valid POI selected - that means
        // 1. Tell "image viewer" about the POI
        QPointF qtFocusStarPos(center.x(), center.y());
        mImageViewerPanel->setPoi(qtFocusStarPos);

        // 2. Change button icon to indicate a valid selection
        setBtnIcon(mSelectFocusStarButton,
                   ":/res/select_star_selected_64x64.png");

        // 3. Switch back to navigation mode
        mImageViewerPanel->setMode(ImageViewerModeT::NAVIGATION);
    } else {
        LOG(warning)
            << "No valid star detected." << std::endl;

        // Just remain in POI mode
    }

    updateFocusFinderMainMenuBar();
}

void MainWindow::onSelectFocusStarPressed() {
    LOG(debug)
        << "MainWindow::onSelectFocusStarPressed... checked: "
        << mSelectFocusStarButton->isChecked() << std::endl;

    if (mSelectFocusStarButton->isChecked()) {
        LOG(debug)
            << "MainWindow::onSelectFocusStarPressed... deselect star..."
            << std::endl;

        // Leaving the POI mode - that means:
        // 1. Deselecting the selected star (if any)
        mImageViewerPanel->clearPoi();

        // 2. Go back to NAVIGATION mode
        mImageViewerPanel->setMode(ImageViewerModeT::NAVIGATION);

        // 3. Change icon back to "no star selected"
        setBtnIcon(mSelectFocusStarButton, ":/res/select_star_64x64.png");

    } else {
        LOG(debug)
            << "MainWindow::onSelectFocusStarPressed... start POI mode..."
            << std::endl;

        // Entering POI mode - that means:
        // 1. Change icon on button to "selecting a star" (in progress)
        setBtnIcon(mSelectFocusStarButton,
                   ":/res/select_star_selecting_64x64.png");

        // 2. Clear old POI (just to make sure)
        mImageViewerPanel->clearPoi();

        // 2. Set "image viewer" to POI select mode
        mImageViewerPanel->setMode(ImageViewerModeT::POI_SELECT);
    }

    updateFocusFinderMainMenuBar();
}

void MainWindow::createSelectFocusStarButton() {
    mSelectFocusStarButton = new AnimMenuButtonT;

    setBtnIcon(mSelectFocusStarButton, ":/res/select_star_64x64.png");

    mSelectFocusStarButton->setCheckable(true);
    mSelectFocusStarButton->setChecked(false);

    connect(mSelectFocusStarButton, &QToolButton::pressed, this,
            &MainWindow::onSelectFocusStarPressed);
    connect(mImageViewerPanel, &ImageViewerPanelT::poiSelectedSignal, this,
            &MainWindow::onPoiSelectedSlot);

    getFocusFinderMainMenuBar()->addWidget(mSelectFocusStarButton);
}

void MainWindow::createImageViewerPanel() {
    mImageViewerPanel = new ImageViewerPanelT(mFfl);

    // Get window size from profile
    connect(mImageViewerPanel, &ImageViewerPanelT::roiClearedSignal, this,
            &MainWindow::onRoiClearedSlot);
    connect(mImageViewerPanel, &ImageViewerPanelT::roiSelectedSignal, this,
            &MainWindow::onRoiSelectedSlot);

    m_ui->layImageViewer->addWidget(mImageViewerPanel);
}

void MainWindow::createFocusFinderMainMenuBar() {
    createManageDeviceProfilesButton();
    createExposureButton();

    auto *spacer1 = new QSpacerItem(10, 10);
    getFocusFinderMainMenuBar()->addItem(spacer1);

    createExposureTimeSelector();

    auto *spacer2 = new QSpacerItem(10, 10);
    getFocusFinderMainMenuBar()->addItem(spacer2);

    createSelectSubFrameButton();

    auto *spacer3 = new QSpacerItem(10, 10);
    getFocusFinderMainMenuBar()->addItem(spacer3);

    createSelectFocusStarButton();

    auto *spacer4 = new QSpacerItem(10, 10);
    getFocusFinderMainMenuBar()->addItem(spacer4);

    auto *spacer5 = new QSpacerItem(10, 10);
    getFocusFinderMainMenuBar()->addItem(spacer5);

    auto *spacer6 = new QSpacerItem(10, 10);
    getFocusFinderMainMenuBar()->addItem(spacer6);

    createStartFocusFinderButton();

    getFocusFinderMainMenuBar()->addStretch();
}

void MainWindow::updateFocusFinderButtonMenuTexts() {
    auto activeProfile = mFfl.getProfileManager()->getActiveProfile();
    bool hasActiveProfile = activeProfile.has_value();

    bool hasCalibrationData = hasActiveProfile && activeProfile.value().hasCalibrationData();

    std::string calibrationMenuStr = (hasCalibrationData ? "Recalibrate..." : "Calibrate...");
    std::string calibrationMenuToolTipStr = (hasCalibrationData ? "Replace the existing calibration by a new one."
                                                                : "Perform an initial calibration.");
    mRecalibrationAction->setText(QString::fromStdString(calibrationMenuStr));
    mRecalibrationAction->setStatusTip(QString::fromStdString(calibrationMenuToolTipStr));
}

void MainWindow::updateFocusFinderMainMenuBar() {
    LOG(debug)
        << "MainWindow::updateFocusFinderMainMenuBar..." << std::endl;

    auto currentCamera = getCurrentCamera();
    // mExposureButton - enabled if
    //   -A "focus finder profile" is set
    //	 -AND camera device is selected
    //	 -AND camera is connected
    //	 -Focus finder not running
    auto activeProfile = mFfl.getProfileManager()->getActiveProfile();
    bool focusFinderRunning = mFfl.getFocusFinderExecutor()->isRunning();

    bool hasActiveProfile = activeProfile.has_value();
    bool cameraSelected = (currentCamera != nullptr);
    bool cameraConnected =
            currentCamera && currentCamera->getParentDevice()->isConnected();

    bool exposureRunning = currentCamera && currentCamera->isExposureRunning();

    LOG(debug) << "exposureRunning: " << exposureRunning << std::endl;

    mExposureButton->setEnabled(
            hasActiveProfile && cameraSelected && cameraConnected
            && !focusFinderRunning);

    // Exposure time selector
    updateExposureTimeSelector();

    // mSelectSubFrameButton - enabled if
    //   -"focus finder profile" is active
    //	 -AND camera device is selected
    //	 [-AND camera device is connected (?)]
    //	 -Focus finder not running
    //	 -Not currently in POI select mode
    //	 -AND a picture is there
    bool inPoiMode = (mImageViewerPanel->getMode()
                      == ImageViewerModeT::POI_SELECT);
    bool hasImage = !mFfl.getLastFrame().isEmpty();

    mSelectSubFrameButton->setEnabled(
            hasActiveProfile && cameraSelected && !focusFinderRunning
            && !inPoiMode && hasImage && !exposureRunning);

    /**
     * mSelectFocusStarButton - enable if
     *  -"focus finder profile" is active
     *	-AND camera device is selected
     *	[-AND camera device is connected (?)]
     *	-Focus finder not running
     *	-Not currently in ROI select mode
     *	-AND a picture is there
     */
    bool inRoiMode = (mImageViewerPanel->getMode()
                      == ImageViewerModeT::ROI_SELECT);

    mSelectFocusStarButton->setEnabled(
            hasActiveProfile && cameraSelected && !focusFinderRunning
            && !inRoiMode && hasImage && !exposureRunning);


    /**
     * mStartFocusFinderButton - enable when
     *  -"focus finder profile" is active
     *  -AND camera device is selected
     *	-AND camera device is connected
     *	-AND focus device is selected
     *  -AND focus device is connected (filter only optional?!)
     *  -AND valid focus star selected
     *  -AND calibration data is available
     *  -AND no exposure is currently running
     */
    auto currentFocus = mFfl.getCurrentFocus();
    bool focusSelected = (currentFocus != nullptr);

    bool focusConnected = currentFocus && currentFocus->getParentDevice()->isConnected();

    bool validFocusStarSelected = mImageViewerPanel->isPoiSet();

    bool readyForCalibration = hasActiveProfile && cameraSelected && cameraConnected
                               && focusSelected && focusConnected
                               && validFocusStarSelected
                               && !exposureRunning;

    // Disable the "focus finder" menu items in case the focus finder is running or the button itself is disabled
    updateFocusFinderButtonMenuTexts();

    bool enableRecalibrationAction = readyForCalibration && !focusFinderRunning;

    mRecalibrationAction->setEnabled(enableRecalibrationAction);

    bool hasCalibrationData = hasActiveProfile && activeProfile.value().hasCalibrationData();
    bool enableEditCalibrationAction = hasCalibrationData && !focusFinderRunning;

    mEditCalibrationDataAction->setEnabled(enableEditCalibrationAction);

    /**
     * NOTE: Problem here is, that disabling the main functionality of this button, also disables the
     *       menu... Therefore, it is enabled when it is at least ready for calibration. If there
     *       is no calibration, it is proposed to the user when the button is pressed. Otherwise,
     *       the focus finder just starts with the calibration available.
     */
    mStartFocusFinderButton->setEnabled(readyForCalibration);


    /**
     * mManageDeviceProfilesButton - enabled if
     *   [-INDI server connected (?)]
     *   -Focus finder not running
     *
     *   TODO: Not sure if this button should even bie disabled in case the focus
     *         finder is running...
     *
     *   TODO: Also disable if any device action is in progress? -> I guess no...
     *         User wants to have control about connecting / disconnecting devices.
     *         A device could behave strange and user maybe wants to disconnect it.
     *         A device could be "busy" all the time and user would never be able
     *         to disconnect.
     */
    mManageDeviceProfilesButton->setEnabled(!focusFinderRunning);
}

void MainWindow::onUpdateProfileSlot(std::optional<FocusFinderProfileT> oldProfile,
                                     std::optional<FocusFinderProfileT> newProfile) {
    LOG(debug)
        << "MainWindow::onUp"
           "dateProfileSlot..." << std::endl;

    updateProfile(std::move(oldProfile), std::move(newProfile));
    updateFocusFinderMainMenuBar();
}

MainWindow::MainWindow() :
        m_ui(new Ui::MainWindow),
        mFfl(*FocusFinderLogicT::get()),    // TODO: Remove mFfl... directly use static function...
        mFocusDevice(nullptr),
        mFilterDevice(nullptr)
{
    // Setup UI
    m_ui->setupUi(this);

    mManageDeviceProfilesDialog = new ManageDeviceProfilesDialogT(mFfl);

    // Register at profile manager to get notified if selected profile / device changes...
    mFfl.getProfileManager()->registerActiveProfileChangedListener(
            [this](std::optional<FocusFinderProfileT> oldProfile,
                   std::optional<FocusFinderProfileT> newProfile) { onUpdateProfileSlot(std::move(oldProfile), std::move(newProfile)); });

    createImageViewerPanel();
    createReportingViewerPanel();
    createImageConverterPanel();
    createFocusCntlPanel();
    createFilterCntlPanel();

    // // TODO: They take too much space this way - find better way...
    // //createHfdViewPanel();
    // //createFwhmViewPanels();

    createFocusFinderMainMenuBar();
    createStatusBar();
    createAboutDialog();

    updateProfile(std::nullopt, mFfl.getProfileManager()->getActiveProfile());


    showDeviceManagerConfigDialog();
}

MainWindow::~MainWindow() {
    LOG(debug) << "Entering MainWindow::~MainWindow..." << std::endl;
    delete mExposureButton;
    delete mExposureLoopAction;
    delete mExposureButtonMenu;

    delete mManageDeviceProfilesButton;
    delete mManageDeviceProfilesDialog;

    delete mSelectSubFrameButton;

    delete mSelectFocusStarButton;
    delete mStartFocusFinderButton;

    delete mFocusCntlPanel;
    delete mRecalibrationAction;
    delete mEditCalibrationDataAction;
    delete mStartFocusFinderButtonMenu;

    delete mCameraConnectionStatusLabel;
    delete mFocusConnectionStatusLabel;
    delete mFilterConnectionStatusLabel;

    delete mImageViewerPanel;
    delete mImageConverterPanel;
    delete mReportingViewerPanel;

    delete mAboutDialog;
}

QHBoxLayout *MainWindow::getFocusFinderMainMenuBar() {
    return m_ui->layFocusFinderMainMenuBar;
}

void MainWindow::createAboutDialog() {
    mAboutDialog = new AboutDialogT(this);
    mAboutDialog->setModal(true);
}

[[maybe_unused]] void MainWindow::on_about() {
//    QString about = QString() +
//        "Version:   " + LIBINDICLIENTPP_VERSION + "\n";
//
    mAboutDialog->show();
}

// TODO: Find better name for this method...
void MainWindow::showDeviceManagerConfigDialog() {
    // Create INDI config dialog (modal)...
    std::shared_ptr<DeviceManagerT> deviceManager = FocusFinderLogicT::get()->getDeviceManager();

    LOG(debug) << "Using DeviceManager: " << DeviceManagerTypeT::asStr(deviceManager->getDeviceManagerType())
               << std::endl;

    mDeviceManagerDialog = DeviceManagerDialogFactoryT::getInstance(this, deviceManager);

    if (!deviceManager->isReady()) {
        LOG(info) << "Device manager connection failed. Bringing up config dialog..." << std::endl;

        //mDeviceManagerDialog->setModal(true);
        mDeviceManagerDialog->open();
    }
}
