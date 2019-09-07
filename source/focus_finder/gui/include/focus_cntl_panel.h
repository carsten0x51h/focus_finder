#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CNTL_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CNTL_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QSignalMapper>

#include "../../common/include/focus_finder_logic.h"

class FocusT;
class QMovie;

namespace Ui {
    class FocusCntlPanel;
}

/**
*  @brief
*/
class FocusCntlPanelT : public QWidget
{
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
	FocusCntlPanelT(QWidget * parent, FocusFinderLogicT & ffl);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FocusCntlPanelT();

signals:
	void deviceConnectedSignal();
	void deviceDisconnectedSignal();
	void focusPositionChangedSignal(int absPos);
	void targetPositionReachedSignal(int absPos);
	void focusMovementAbortedSignal(int currentPos);

protected slots:
	void setFocusAnimationIcon(int /*frame*/);
	void onFocusPositionChangeRequest(int inSteps); // One of the +/- buttons pressed
	void onFocusAbortRequest();                     // Abort button pressed

	void onDeviceConnectedSlot();
	void onDeviceDisconnectedSlot();
	void onFocusPositionChangedSlot(int absPos);
	void onTargetPositionReachedSlot(int absPos);
	void onFocusMovementAbortedSlot(int currentPos);

protected:
	const QScopedPointer<Ui::FocusCntlPanel> m_ui;

private:
	void updateCurrentPos(int absPos);
	void updateProfile();
	std::shared_ptr<FocusT> getFocus() const;

	void startAnimation();
	void stopAnimation();

	QMovie * mMovie;

	FocusFinderLogicT & mFfl;
	QSignalMapper * mStepSignalMapper;
	std::shared_ptr<FocusT> mFocusDevice;

	boost::signals2::connection mDeviceConnectedConnection;
	boost::signals2::connection mDeviceDisconnectedConnection;
	boost::signals2::connection mFocusPositionChangedConnection;
	boost::signals2::connection mTargetPositionReachedConnection;
	boost::signals2::connection mFocusMovementAbortedConnection;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CNTL_PANEL_H_*/
