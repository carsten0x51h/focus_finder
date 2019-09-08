#include <QWidget>

#include "include/focus_curve_recorder_panel.h"

#include "../common/include/logging.h"

#include "ui_focus_curve_recorder_panel.h"

//TODO: Next step: Add in mai_window...

FocusCurveRecorderPanelT::FocusCurveRecorderPanelT(QWidget * parent, FocusFinderLogicT & ffl) : QWidget(parent),
                                                                                        m_ui(new Ui::FocusCurveRecorderPanel),
                                                                                        mFfl(ffl)
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

    reset();
}

FocusCurveRecorderPanelT::~FocusCurveRecorderPanelT()
{

}

void FocusCurveRecorderPanelT::reset() {

}
