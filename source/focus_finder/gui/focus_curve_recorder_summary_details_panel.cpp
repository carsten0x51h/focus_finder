#include <memory>

#include <QWidget>
#include <QMovie>

#include "include/focus_curve_recorder_summary_details_panel.h"

#include "../common/include/logging.h"
#include "../common/include/focus_curve_recorder_logic.h"

#include "ui_focus_curve_recorder_summary_details_panel.h"

FocusCurveRecorderSummaryDetailsPanelT::FocusCurveRecorderSummaryDetailsPanelT(QWidget * parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic) : QWidget(parent),
                                                                                        m_ui(new Ui::FocusCurveRecorderSummaryDetailsPanel),
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

FocusCurveRecorderSummaryDetailsPanelT::~FocusCurveRecorderSummaryDetailsPanelT()
{
}

void FocusCurveRecorderSummaryDetailsPanelT::reset()
{
}
