#include <QWidget>

#include <memory>
#include <cmath>

#include "include/focus_curve_view_panel.h"
#include "include/focus_curve_view_widget.h"

#include "../common/include/logging.h"
#include "../common/include/focus_curve.h"

#include "ui_focus_curve_view_panel.h"


FocusCurveViewPanelT::FocusCurveViewPanelT(QWidget * parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic) : QWidget(parent),
							       m_ui(new Ui::FocusCurveViewPanel), mFocusCurveRecorderLogic(focusCurveRecorderLogic)
{
    // Setup UI
    m_ui->setupUi(this);

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(100);
    sizePolicy.setVerticalStretch(100);

    mFocusCurveWidget = new FocusCurveViewWidgetT(m_ui->widget, mFocusCurveRecorderLogic);
    mFocusCurveWidget->setSizePolicy(sizePolicy);
    m_ui->layFocusCurveViewWidget->addWidget(mFocusCurveWidget, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);
    
    reset();
}

FocusCurveViewPanelT::~FocusCurveViewPanelT()
{
	// TODO: Cleanup required??
	//delete mHfdWidget;
}

void FocusCurveViewPanelT::reset() {
  mFocusCurveWidget->reset();
}

void FocusCurveViewPanelT::update() {
  mFocusCurveWidget->update();
}

// FocusMeasureTypeT::TypeE FocusCurveViewPanelT::getFocusMeasureType() const {
//   return mFocusCurveWidget->getFocusMeasureType();
// }

// void FocusCurveViewPanelT::setFocusMeasureType(FocusMeasureTypeT::TypeE focusMeasureType) {
//   mFocusCurveWidget->setFocusMeasureType(focusMeasureType);
// }

// void FocusCurveViewPanelT::setFocusCurve(std::shared_ptr<const FocusCurveT> focusCurve) {
// 	LOG(debug) << "FocusCurveViewPanelT::setFocusCurve..." << std::endl;

// 	mFocusCurveWidget->setFocusCurve(focusCurve);

// //	std::string text = "-";
// //	if (hfd.valid()) {
// //		std::ostringstream oss;
// //		oss.precision(4);
// //		oss << hfd.getValue();
// //		text = oss.str();
// //	}
// //
// //	m_ui->lblHfdValue->setText(QString::fromStdString(text));
// }

