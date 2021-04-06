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
 *  along with this progr   am ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

#include <QWidget>

#include <memory>
#include <cmath>

#include "include/fwhm_view_panel.h"
#include "include/fwhm_view_widget.h"

#include "ui_fwhm_view_panel.h"


FwhmViewPanelT::FwhmViewPanelT(QWidget *parent, FocusFinderLogicT &ffl) : QWidget(parent),
                                                                          m_ui(new Ui::FwhmViewPanel),
                                                                          mFfl(ffl) {
    // Setup UI
    m_ui->setupUi(this);

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(100);
    sizePolicy.setVerticalStretch(100);

    mFwhmWidget = new FwhmViewWidgetT(m_ui->widget);
    mFwhmWidget->setSizePolicy(sizePolicy);
    m_ui->layFwhmViewWidget->addWidget(mFwhmWidget, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);

    reset();
}

FwhmViewPanelT::~FwhmViewPanelT() {
    // TODO: Cleanup required??
    //delete mFwhmWidget;
}

void FwhmViewPanelT::reset() {
    mFwhmWidget->reset();
}

void FwhmViewPanelT::setFwhm(const FwhmT &fwhm) {
    LOG(debug) << "FwhmViewPanelT::setFwhm...mFwhm valid? " << fwhm.valid() << std::endl;

    mFwhmWidget->setFwhm(fwhm);

    // Set Fwhm text
    std::string text = "-";

    if (fwhm.valid()) {
        std::ostringstream oss;
        oss.precision(4);
        oss << fwhm.getValue();
        text = oss.str();
    }

    m_ui->lblFwhmValue->setText(QString::fromStdString(text));
}
