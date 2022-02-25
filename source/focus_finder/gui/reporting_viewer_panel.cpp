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

#include <QStandardItem>
#include <QList>

#include <memory>
#include <ctime>

#include "include/reporting_viewer_panel.h"

#include "../common/include/logging.h"
#include "../common/include/reporting.h"

#include "ui_reporting_viewer_panel.h"

ReportingViewerPanelT::ReportingViewerPanelT(QWidget *parent) :
        QWidget(parent), m_ui(new Ui::ReportingViewerPanel) {
    // Setup UI
    m_ui->setupUi(this);

    mDataModel = new QStandardItemModel(this);
    mDataModel->setColumnCount(5);

    mDataModel->setHeaderData(0, Qt::Horizontal, QObject::tr(""));
    mDataModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Time"));
    mDataModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Sender"));
    mDataModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Title"));
    mDataModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Details"));

    m_ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_ui->tableView->setModel(mDataModel);

    m_ui->tableView->setColumnWidth(0, 15);
    m_ui->tableView->setColumnWidth(1, 180);
    m_ui->tableView->setColumnWidth(2, 100);
    m_ui->tableView->setColumnWidth(3, 300);
    m_ui->tableView->setColumnWidth(4, 600);


    connect(mDataModel, &QStandardItemModel::rowsInserted, this,
            &ReportingViewerPanelT::onModelRowsInsertedSlot);


    // Register to reporting to receive new reports
    mNewReportConnection =
            ReportingT::registerNewReportListener(
                    [&](const ReportingDatasetT &reportingDataset) { emit newReportSignal(reportingDataset); });

    qRegisterMetaType<ReportingDatasetT>("ReportingDatasetT");

    connect(this, &ReportingViewerPanelT::newReportSignal, this,
            &ReportingViewerPanelT::onNewReportSlot);
}

ReportingViewerPanelT::~ReportingViewerPanelT() {
    ReportingT::unregisterNewReportListener(mNewReportConnection);

    delete mDataModel;
}

// See https://stackoverflow.com/questions/19102625/qtableview-scroll-to-new-added-row
void ReportingViewerPanelT::onModelRowsInsertedSlot(const QModelIndex &/*parent*/, int start, int /*end*/) {
    m_ui->tableView->scrollTo(mDataModel->index(start, 0));
}

void ReportingViewerPanelT::onNewReportSlot(
        const ReportingDatasetT &reportingDataset) {

    LOG(debug) << "ReportingViewerPanelT::onNewReportSlot..." << std::endl;

    addEntry(reportingDataset);
}

void ReportingViewerPanelT::addEntry(
        const ReportingDatasetT &reportingDataset) {

    LOG(debug) << "ReportingViewerPanelT::addEntry..." << reportingDataset
               << std::endl;

    // TODO: Keep only max. N entries in the list (delete oldest)...

    QList<QStandardItem *> newRow;

    std::time_t timeTmp = reportingDataset.getTime();


    newRow.append(new QStandardItem(QIcon(":/res/red_dot_64x64.png"), QString()));

    newRow.append(
            new QStandardItem(
                    QString::fromStdString(
                            std::ctime(&timeTmp))));

    newRow.append(
            new QStandardItem(
                    QString::fromStdString(reportingDataset.getSender())));

    newRow.append(
            new QStandardItem(
                    QString::fromStdString(reportingDataset.getTitle())));
    newRow.append(
            new QStandardItem(
                    QString::fromStdString(reportingDataset.getDetails())));

    mDataModel->appendRow(newRow);
}
