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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_MAIN_WINDOW_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_MAIN_WINDOW_H_ SOURCE_FOCUS_FINDER_GUI_INCLUDE_MAIN_WINDOW_H_

// Qt includes
#include <QHBoxLayout>
#include <QMainWindow>
#include <QScopedPointer>
#include <QPushButton>
#include <QLabel>
#include <QRect>
#include <QComboBox>

// std includes
#include <memory>
#include <optional>


#include "anim_menu_button.h"

#include "../../common/include/focus_finder_logic.h"
#include "../../common/include/device_manager.h"
#include "../../common/include/image.h"
#include "../../common/include/focus_curve_record.h"

class CameraInterfaceT;

class FocusInterfaceT;

class FilterInterfaceT;

class ImageViewerPanelT;

class ImageConverterPanelT;

class FocusCntlPanelT;

class FilterCntlPanelT;

class HfdViewPanelT;

class FwhmViewPanelT;

class StarDetailsPanelT;

class ManageDeviceProfilesDialogT;

class AboutDialogT;

class ReportingViewerPanelT;

class FocusFinderProfileT;

namespace Ui {
    class MainWindow;
}


/**
*  @brief
*    Main window of the indigui example
*/
class MainWindow : public QMainWindow {
Q_OBJECT


public:
    /**
    *  @brief
    *    Constructor
    */
    MainWindow();

    /**
    *  @brief
    *    Destructor
    */
    ~MainWindow() override;

protected slots:

    void on_about();

    void onRoiClearedSlot();

    void onRoiSelectedSlot(const QRect &roiRect, const QImage &roiImage);

    void onPoiSelectedSlot(const QPoint &poi);

    //void onUpdateProfileSlot();
    void
    onUpdateProfileSlot(std::optional<FocusFinderProfileT> oldProfile, std::optional<FocusFinderProfileT> newProfile);

    void onExposureTimeSelectionChangedSlot();

    // Focus finder
    void onFocusFinderStarted();

    void onFocusFinderFinished();

    void onFocusFinderCancelled();

    void onFocusFinderProgressUpdate(float progress, const QString &msg, const std::shared_ptr<FocusCurveRecordT>& record);


protected:
    const QScopedPointer<Ui::MainWindow> m_ui;

private:
    static std::shared_ptr<CameraInterfaceT> getCurrentCamera();

    static std::shared_ptr<FocusInterfaceT> getCurrentFocus();

    static std::shared_ptr<FilterInterfaceT> getCurrentFilter();


    void showDeviceManagerConfigDialog();

    void setExposureButtonState(bool isRunning); // Helper

    void updateCurrentCameraConnectionStateUI();

    void updateCurrentFocusConnectionStateUI();

    void updateCurrentFilterConnectionStateUI();

    static void setStatusIcon(QLabel *btn, const std::string &filename, const std::string &toolTip);

    static void setBtnIcon(QAbstractButton *btn, const std::string &filename);

    void onManageDeviceProfiles();

    void onSelectDeviceProfile();

    void onDeviceProfileConnected();

    void updateImage();

    void createManageDeviceProfilesButton();

    void createExposureButton();

    void createExposureTimeSelector();

    void createSelectSubFrameButton();

    void createSelectFocusStarButton();

    void createStartFocusFinderButton();

    void createImageViewerPanel();

    void createReportingViewerPanel();

    void createImageConverterPanel();

    void createFocusCntlPanel();

    void createFilterCntlPanel();

    void createHfdViewPanel();

    void createFwhmViewPanels();

    void createStarDetailsPanel();

    void createFocusFinderMainMenuBar();

    void createStatusBar();

    void createAboutDialog();

    void updateCameraDevice(const std::shared_ptr<CameraInterfaceT>& oldCameraInterface,
                            const std::shared_ptr<CameraInterfaceT>& newCameraInterface);

    void updateFocusDevice(const std::shared_ptr<FocusInterfaceT>& oldFocusInterface,
                           const std::shared_ptr<FocusInterfaceT>& newFocusInterface);

    void updateFilterDevice(const std::shared_ptr<FilterInterfaceT>& oldFilterInterface,
                            const std::shared_ptr<FilterInterfaceT>& newFilterInterface);

    void updateProfile(std::optional<FocusFinderProfileT> oldProfile, std::optional<FocusFinderProfileT> newProfile);

    void updateExposureTime();

    void updateExposureTimeSelector();

    void updateFocusFinderButtonMenuTexts();

    void updateFocusFinderMainMenuBar();

    // Business logic
    //TODO: Make it static ?!?!? in main... independent from GUI!?
    FocusFinderLogicT &mFfl;

    // Image viewer
    ImageViewerPanelT *mImageViewerPanel;

    // Image converter panel
    ImageConverterPanelT *mImageConverterPanel;

    // Add reporting viewer
    ReportingViewerPanelT *mReportingViewerPanel;

    // Manage device profiles
    std::shared_ptr<QDialog> mDeviceManagerDialog;
    QPushButton *mManageDeviceProfilesButton;
    ManageDeviceProfilesDialogT *mManageDeviceProfilesDialog;

    // TODO: Focusing profiles...
    //List<QAction *> selectFocusingProfileActions; // TODO: Good idea?

    // Exposure button
    AnimMenuButtonT *mExposureButton;
    QAction *mExposureLoopAction;
    QMenu *mExposureButtonMenu;

    // Exposure time
    QComboBox *mExposureTimeCbx;

    // Select sub frame button
    AnimMenuButtonT *mSelectSubFrameButton;

    // Select focus star button
    AnimMenuButtonT *mSelectFocusStarButton;

    // "find focus" button - calibration menu
    AnimMenuButtonT *mStartFocusFinderButton;

    QAction *mRecalibrationAction;
    QAction *mEditCalibrationDataAction;
    QMenu *mStartFocusFinderButtonMenu;

    // Device icons in status bar
    QLabel *mCameraConnectionStatusLabel;
    QLabel *mFocusConnectionStatusLabel;
    QLabel *mFilterConnectionStatusLabel;

    FocusCntlPanelT *mFocusCntlPanel;
    FilterCntlPanelT *mFilterCntlPanel;

    HfdViewPanelT *mHfdViewPanel;
    FwhmViewPanelT *mFwhmHorzViewPanel;
    FwhmViewPanelT *mFwhmVertViewPanel;
    StarDetailsPanelT *mStarDetailsPanel;

    AboutDialogT *mAboutDialog;

    std::chrono::milliseconds mExposureTime;

    QHBoxLayout *getFocusFinderMainMenuBar();

    // Device status LEDs
    void onCameraDeviceConnected();

    void onCameraDeviceConnecting();

    void onCameraDeviceDisconnected();

    void onFocusDeviceConnected();

    void onFocusDeviceConnecting();

    void onFocusDeviceDisconnected();

    void onFilterDeviceConnected();

    void onFilterDeviceConnecting();

    void onFilterDeviceDisconnected();


    void onExposurePressed();

    void onExposureCycleFinished(bool lastExposure);

    void onFrameTransferUpd(double percentage);

    void onExposureCancelled();

    void onExposureTimerUpd(double t);

    void onExposureDelayTimerUpd(double t);

    void onExposureLoopPressed();

    void onSelectSubFramePressed();

    void onSelectFocusStarPressed();

    void onStartFocusFinderPressed();

    void onRecalibrationPressed();

    void onEditCalibrationDataPressed();

    void onImageConverterValueChanged();

    // Store current devices
    // TODO: Why actually storing a ptr? Why not always querying the current one from the focus finder logic? Shouldn't this logic hold pointers to this logic instead? But the problem is: when a device changes (profile change), there might be listeners registerd to this device. -> profile change should call a function which provides old and new device ptr so that each device listener has the chance to be unregister it's listeners. In any case, the respective component (e.g. main window) still needs to react on the device change.The point in time when the callback which informs about the change is very important! This is a bigger change...
    std::shared_ptr<DeviceT> mFocusDevice;
    std::shared_ptr<DeviceT> mFilterDevice;

    // Camera listener handles
    boost::signals2::connection mCameraDeviceConnectedConnection;
    boost::signals2::connection mCameraDeviceConnectingConnection;
    boost::signals2::connection mCameraDeviceDisconnectedConnection;
    boost::signals2::connection mExposureCycleFinishedConnection;
    boost::signals2::connection mExposureCancelledConnection;
    boost::signals2::connection mFrameTransferUpdConnection;
    boost::signals2::connection mFrameExposureTimerUpdConnection;
    boost::signals2::connection mFrameExposureDelayTimerUpdConnection;

    // Focus listener handles
    boost::signals2::connection mFocusDeviceConnectedConnection;
    boost::signals2::connection mFocusDeviceConnectingConnection;
    boost::signals2::connection mFocusDeviceDisconnectedConnection;

    // Filter listener handles
    boost::signals2::connection mFilterDeviceConnectedConnection;
    boost::signals2::connection mFilterDeviceConnectingConnection;
    boost::signals2::connection mFilterDeviceDisconnectedConnection;

signals:

    // Device connections
    void cameraDeviceConnectedSignal();

    void cameraDeviceConnectingSignal();

    void cameraDeviceDisconnectedSignal();

    // Focus
    void focusDeviceConnectedSignal();

    void focusDeviceConnectingSignal();

    void focusDeviceDisconnectedSignal();

    // Filter
    void filterDeviceConnectedSignal();

    void filterDeviceConnectingSignal();

    void filterDeviceDisconnectedSignal();

    // Camera
    void exposureCycleFinishedSignal(bool lastExposure);

    void frameTransferUpdSignal(double percentage);

    void exposureCancelledSignal();

    void exposureTimerUpdSignal(double t);

    void exposureDelayTimerUpdSignal(double t);

    void startExposureSignal();

    // Focus finder
    void focusFinderStartedSignal();

    void focusFinderProgressUpdateSignal(float progress, const QString &msg, std::shared_ptr<FocusCurveRecordT> record);

    void focusFinderFinishedSignal();

    void focusFinderCancelledSignal();
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_MAIN_WINDOW_H_*/
