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

#include <memory>
#include <string>
#include <iomanip>
#include <sstream>

#include <QWidget>
#include <QHBoxLayout>
#include <utility>

#include "include/focus_curve_recorder_point_details_panel.h"

#include "../common/include/logging.h"
#include "../common/include/focus_curve_record.h"
#include "../common/include/focus_curve_recorder_logic.h"

#include "ui_focus_curve_recorder_point_details_panel.h"

FocusCurveRecorderPointDetailsPanelT::FocusCurveRecorderPointDetailsPanelT(QWidget *parent,
                                                                           std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic)
        : QWidget(parent),
          m_ui(new Ui::FocusCurveRecorderPointDetailsPanel),
          mFocusCurveRecorderLogic(std::move(focusCurveRecorderLogic)) {
    // Setup UI
    m_ui->setupUi(this);

//    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    sizePolicy.setHorizontalStretch(100);
//    sizePolicy.setVerticalStretch(100);
//
//    mFocusCurveWidget = new FocusCurveViewWidgetT(m_ui->widget);
//    mFocusCurveWidget->setSizePolicy(sizePolicy);
//    m_ui->layFocusCurveViewWidget->addWidget(mFocusCurveWidget, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);

    // Add the focus curve viewer panel
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(100);
    sizePolicy.setVerticalStretch(100);

    // mFocusCurveViewPanel = new FocusCurveViewPanelT(m_ui->widget, mFocusCurveRecorderLogic);
    // mFocusCurveViewPanel->setSizePolicy(sizePolicy);
    // m_ui->layFocusCurveViewPanel->addWidget(mFocusCurveViewPanel, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);

    reset();
}

FocusCurveRecorderPointDetailsPanelT::~FocusCurveRecorderPointDetailsPanelT() = default;

void FocusCurveRecorderPointDetailsPanelT::reset() {
}


void FocusCurveRecorderPointDetailsPanelT::setPointDetails(const std::shared_ptr<FocusCurveRecordT>& focusCurveRecord) {
    std::stringstream ss;

    // TODO: focusCurveRecord->getDateTime() changed to time_point...
    //       --> adapt code below...
    // std::time_t t = focusCurveRecord->getDateTime();
    // char mbstr[100];
    // std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(& t));
    // m_ui->lblDateTime->setText(QString::fromStdString(std::string(mbstr)));
    // ss.str(std::string());


    ss << std::fixed << std::setprecision(2) << focusCurveRecord->getHfd().getValue();
    m_ui->lblHfd->setText(QString::fromStdString(ss.str()));
    ss.str(std::string());


    // TODO: Convert to arcsec?
    ss << std::fixed << std::setprecision(2) << focusCurveRecord->getFwhmVert().getValue();
    m_ui->lblFwhmVert->setText(QString::fromStdString(ss.str()));
    ss.str(std::string());


    // TODO: Convert to arcsec?
    ss << std::fixed << std::setprecision(2) << focusCurveRecord->getFwhmHorz().getValue();
    m_ui->lblFwhmHorz->setText(QString::fromStdString(ss.str()));
    ss.str(std::string());


    ss << focusCurveRecord->getCurrentAbsoluteFocusPos();
    m_ui->lblFocusPos->setText(QString::fromStdString(ss.str()));
    ss.str(std::string());


    ss << std::fixed << std::setprecision(2) << focusCurveRecord->getSnr();
    m_ui->lblSnr->setText(QString::fromStdString(ss.str()));
    ss.str(std::string());


    // TODO: Where is this info?
    //m_ui->lblIsOutlier->setText(QString::fromStdString(focusCurveRecord->getIsOutlier()));
}
