#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_REPORTING_VIEWER_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_REPORTING_VIEWER_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QStandardItemModel>

#include <boost/signals2.hpp>

#include "../../common/include/reporting_dataset.h"

namespace Ui {
    class ReportingViewerPanel;
}

/**
*  @brief
*/
class ReportingViewerPanelT : public QWidget
{
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
	ReportingViewerPanelT(QWidget * parent);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~ReportingViewerPanelT();

signals:
	void newReportSignal(const ReportingDatasetT & reportingDataset);

protected slots:
	void onNewReportSlot(const ReportingDatasetT & reportingDataset);
	void onModelRowsInsertedSlot(const QModelIndex & parent, int start, int end);

protected:
	const QScopedPointer<Ui::ReportingViewerPanel> m_ui;

private:
	void addEntry(const ReportingDatasetT & reportingDataset);

	boost::signals2::connection mNewReportConnection;

	QStandardItemModel* mDataModel;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_REPORTING_VIEWER_PANEL_H_*/
