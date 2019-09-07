
#pragma once


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


#include "anim_menu_button.h"

#include "../../common/include/focus_finder_logic.h"
#include "../../common/include/device_manager.h"
#include "../../common/include/image.h"
#include "../../common/include/focus_finder_record.h"

class CameraT;
class FocusT;
class FilterT;
class ImageViewerPanelT;
class ImageConverterPanelT;
class FocusCntlPanelT;
class FilterCntlPanelT;
class HfdViewPanelT;
class FwhmViewPanelT;
class ManageDeviceProfilesDialogT;
class AboutDialogT;
class ReportingViewerPanelT;

class FocusCurveViewPanelT;

namespace Ui {
    class MainWindow;
}

/**
*  @brief
*    Main window of the indigui example
*/
class MainWindow : public QMainWindow
{
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
    virtual ~MainWindow();

protected slots:
    void on_about();
    void onRoiClearedSlot();
    void onRoiSelectedSlot(const QRect & roiRect, const QImage & roiImage);
    void onPoiSelectedSlot(const QPoint & poi);
    void onUpdateProfileSlot();
    void onExposureTimeSelectionChangedSlot();

    // Focus finder
    void onFocusFinderStarted();
    void onFocusFinderFinished();
    void onFocusFinderCancelled();
    void onFocusFinderProgressUpdate(float progress, const QString & msg, std::shared_ptr<FocusFinderRecordT> record);


protected:
    const QScopedPointer<Ui::MainWindow> m_ui;

private:
    void setExposureButtonState(bool isRunning); // Helper

    void updateCurrentCameraConnectionStateUI();
    void updateCurrentFocusConnectionStateUI();
    void updateCurrentFilterConnectionStateUI();

    static void setStatusIcon(QLabel * btn, const std::string & filename, const std::string & toolTip);
    static void setBtnIcon(QAbstractButton * btn, const std::string & filename);

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
    void createFocusFinderMainMenuBar();
  	void createStatusBar();
  	void createFocusCurveViewPanel();
  	void createAboutDialog();

  	void updateCameraDevice();
  	void updateFocusDevice();
  	void updateFilterDevice();
  	void updateProfile();
  	void updateExposureTime();
  	void updateExposureTimeSelector();
  	void updateFocusFinderMainMenuBar();

    // Business logic
    FocusFinderLogicT mFfl;

    // Image viewer
    ImageViewerPanelT * mImageViewerPanel;

    // Image converter panel
	ImageConverterPanelT * mImageConverterPanel;

	// Add reporting viewer
	ReportingViewerPanelT * mReportingViewerPanel;

    // Manage device profiles
    QPushButton * mManageDeviceProfilesButton;
	ManageDeviceProfilesDialogT * mManageDeviceProfilesDialog;

	// TODO: Focusing profiles...
	//List<QAction *> selectFocusingProfileActions; // TODO: Good idea?

    // Exposure button
	AnimMenuButtonT * mExposureButton;
	QAction * mExposureLoopAction;
	QMenu* mExposureButtonMenu;

	// Exposure time
	QComboBox * mExposureTimeCbx;

	// Select sub frame button
	AnimMenuButtonT * mSelectSubFrameButton;

	// Select focus star button
	AnimMenuButtonT * mSelectFocusStarButton;

	// Start to find focus
	AnimMenuButtonT * mStartFocusFinderButton;

	// Device icons in status bar
	QLabel *mCameraConnectionStatusLabel;
	QLabel *mFocusConnectionStatusLabel;
	QLabel *mFilterConnectionStatusLabel;

	FocusCntlPanelT * mFocusCntlPanel;
	FilterCntlPanelT * mFilterCntlPanel;

	HfdViewPanelT * mHfdViewPanel;
	FwhmViewPanelT * mFwhmHorzViewPanel;
	FwhmViewPanelT * mFwhmVertViewPanel;

	FocusCurveViewPanelT * mFocusCurveViewPanel;

	AboutDialogT * mAboutDialog;

	std::chrono::milliseconds mExposureTime;

	QHBoxLayout * getFocusFinderMainMenuBar();

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

    void onImageConverterValueChanged();

    // Store current devices
    std::shared_ptr<CameraT> mCameraDevice;
    std::shared_ptr<FocusT> mFocusDevice;
    std::shared_ptr<FilterT> mFilterDevice;

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
	void focusFinderProgressUpdateSignal(float progress, const QString & msg, std::shared_ptr<FocusFinderRecordT> record);
	void focusFinderFinishedSignal();
	void focusFinderCancelledSignal();
};
