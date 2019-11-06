#include <memory>
#include <string>
#include <iomanip>
#include <sstream>

#include <QWidget>
#include <QHBoxLayout>

#include "include/focus_curve_recorder_point_details_panel.h"

#include "../common/include/logging.h"
#include "../common/include/focus_curve_record.h"
#include "../common/include/focus_curve_recorder_logic.h"

#include "ui_focus_curve_recorder_point_details_panel.h"

FocusCurveRecorderPointDetailsPanelT::FocusCurveRecorderPointDetailsPanelT(QWidget * parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic) : QWidget(parent),
                                                                                        m_ui(new Ui::FocusCurveRecorderPointDetailsPanel),
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

FocusCurveRecorderPointDetailsPanelT::~FocusCurveRecorderPointDetailsPanelT()
{
}

void FocusCurveRecorderPointDetailsPanelT::reset()
{
}


void FocusCurveRecorderPointDetailsPanelT::setPointDetails(std::shared_ptr<FocusCurveRecordT> focusCurveRecord)
{
  std::stringstream ss;


  std::time_t t = focusCurveRecord->getDateTime();
  char mbstr[100];
  std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(& t));
  m_ui->lblDateTime->setText(QString::fromStdString(std::string(mbstr)));
  ss.str(std::string());

  
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
