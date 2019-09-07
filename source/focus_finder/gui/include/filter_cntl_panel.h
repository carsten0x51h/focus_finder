#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FILTER_CNTL_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FILTER_CNTL_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QSignalMapper>

#include "../../common/include/focus_finder_logic.h"

class FilterT;
class QMovie;

namespace Ui {
    class FilterCntlPanel;
}

/**
*  @brief
*/
class FilterCntlPanelT : public QWidget
{
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
	FilterCntlPanelT(QWidget * parent, FocusFinderLogicT & ffl);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FilterCntlPanelT();

signals:
	void deviceConnectedSignal();
	void deviceDisconnectedSignal();
	void filterPositionChangedSignal(int currentPos);
	void targetPositionReachedSignal(int targetPos);
	void filterMovementAbortedSignal(int currentPos);

protected slots:
	void setFilterAnimationIcon(int /*frame*/);
	void onFilterPositionChangeRequest(int targetIdx);
//	void onFocusAbortRequest();                     // Abort button pressed
//
	void onDeviceConnectedSlot();
	void onDeviceDisconnectedSlot();
	void onFilterPositionChangedSlot(int currentPos);
	void onTargetPositionReachedSlot(int targetPos);
	void onFilterMovementAbortedSlot(int currentPos);

protected:
	const QScopedPointer<Ui::FilterCntlPanel> m_ui;

private:
	void updateProfile();
	std::shared_ptr<FilterT> getFilter() const;

	void startAnimation();
	void stopAnimation();

	QMovie * mMovie;

	FocusFinderLogicT & mFfl;
	std::shared_ptr<FilterT> mFilterDevice;

	boost::signals2::connection mDeviceConnectedConnection;
	boost::signals2::connection mDeviceDisconnectedConnection;
	boost::signals2::connection mFilterPositionChangedConnection;
	boost::signals2::connection mTargetPositionReachedConnection;
	boost::signals2::connection mFilterMovementAbortedConnection;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FILTER_CNTL_PANEL_H_*/
