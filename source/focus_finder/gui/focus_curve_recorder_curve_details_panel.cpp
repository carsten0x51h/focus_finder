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

#include <QWidget>
#include <QHBoxLayout>

#include "include/focus_curve_recorder_curve_details_panel.h"

#include "../common/include/logging.h"
#include "../common/include/focus_curve_recorder_logic.h"

#include "ui_focus_curve_recorder_curve_details_panel.h"

FocusCurveRecorderCurveDetailsPanelT::FocusCurveRecorderCurveDetailsPanelT(QWidget * parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic) : QWidget(parent),
                                                                                        m_ui(new Ui::FocusCurveRecorderCurveDetailsPanel),
                                                                                        mFocusCurveRecorderLogic(focusCurveRecorderLogic)
{
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

FocusCurveRecorderCurveDetailsPanelT::~FocusCurveRecorderCurveDetailsPanelT()
{
}

void FocusCurveRecorderCurveDetailsPanelT::reset()
{
}

void FocusCurveRecorderCurveDetailsPanelT::setCurveDetails(std::shared_ptr<const FocusCurveT> focusCurve)
{
  std::stringstream ss;

  std::time_t t = focusCurve->getDateTime();
  char mbstr[100];
  std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(& t));
  m_ui->lblDateTime->setText(QString::fromStdString(std::string(mbstr)));
  ss.str(std::string());

  
  ss << std::fixed << FocusMeasureTypeT::asStr(focusCurve->getFocusMeasureType());
  m_ui->lblFocusMeasure->setText(QString::fromStdString(ss.str()));
  ss.str(std::string());


  ss << std::fixed << FocusCurveTypeT::asStr(focusCurve->getFocusCurveType());
  m_ui->lblCurveShape->setText(QString::fromStdString(ss.str()));
  ss.str(std::string());

  
  ss << std::fixed << focusCurve->getCurveFitSummary().matchedDataPoints.size();
  m_ui->lblNumMatchedDataPoints->setText(QString::fromStdString(ss.str()));
  ss.str(std::string());


  ss << std::fixed << focusCurve->getCurveFitSummary().outliers.size();
  m_ui->lblNumOutliers->setText(QString::fromStdString(ss.str()));
  ss.str(std::string());
  

  // // TODO: FocusCurveT needs to support this...
  // ss << std::fixed << focusCurve->getStdDev();
  // m_ui->lblStdDev->setText(QString::fromStdString(ss.str()));
  // ss.str(std::string());


  // // TODO: FocusCurveT needs to support this...
  // ss << std::fixed << focusCurve->getBestFocusPos();
  // m_ui->lblBestFocusPos->setText(QString::fromStdString(ss.str()));
  // ss.str(std::string());

  const CurveParmsT & curveParms = focusCurve->getCurveParms();
    
  m_ui->tblCurveParams->setColumnCount(2);
  m_ui->tblCurveParams->setRowCount(curveParms.size());
    
  for (size_t idx = 0; idx < curveParms.size(); ++idx) {
    const CurveParmT & parm = curveParms.get(idx);

    // TODO: Memory leak?
    QTableWidgetItem * nameItem = new QTableWidgetItem(QString::fromStdString(parm.getName()));
    nameItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    m_ui->tblCurveParams->setItem(idx, 0, nameItem);
    
    // TODO: Memory leak?
    QTableWidgetItem * valueItem = new QTableWidgetItem(tr("%1").arg(parm.getValue()));
    valueItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    m_ui->tblCurveParams->setItem(idx, 1, valueItem);    
  }
  
}
