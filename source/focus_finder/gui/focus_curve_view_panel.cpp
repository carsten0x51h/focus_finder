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

#include <QWidget>

#include <memory>
#include <cmath>

#include "include/focus_curve_view_panel.h"
#include "include/focus_curve_view_widget.h"

#include "ui_focus_curve_view_panel.h"


FocusCurveViewPanelT::FocusCurveViewPanelT(QWidget *parent,
                                           std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic) : QWidget(
        parent),
                                                                                                                m_ui(new Ui::FocusCurveViewPanel),
                                                                                                                mFocusCurveRecorderLogic(
                                                                                                                        focusCurveRecorderLogic) {
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

FocusCurveViewPanelT::~FocusCurveViewPanelT() {
    // TODO: Cleanup required??
    //delete mHfdWidget;
}

void FocusCurveViewPanelT::reset() {
    mFocusCurveWidget->reset();
}

void FocusCurveViewPanelT::update() {
    mFocusCurveWidget->update();
}

void FocusCurveViewPanelT::drawCurveHack(std::shared_ptr<FocusCurveT> focusCurve) {
    mFocusCurveWidget->drawCurveHack(focusCurve);
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

