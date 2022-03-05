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
 *  along with this progr   am ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

#include <QWidget>

#include <memory>
#include <cmath>

#include "../common/include/event_bus.h"
#include "../common/include/events/new_frame_event.h"

#include "include/star_details_panel.h"

#include "ui_star_details_panel.h"

// TODO: Maybe rename to StarDetailsViewPanelT?
StarDetailsPanelT::StarDetailsPanelT(QWidget *parent) : QWidget(parent),
                                                        m_ui(new Ui::StarDetailsPanel) {
    // Setup UI
    m_ui->setupUi(this);

    // TODO: Maybe move this out of here? In FWHM ViewPanel this won't work since there are two instances... one for HORZ and one for VERT..
    // --> Maybe one "parent" panel for FWHM to combine vert + horz + avg? No one wants to control both separately! -> Then, using the event bus
    // in the In the view panel would also work for the FWHM panel.
    boost::function<void(NewFrameEventT)> g = boost::bind(& StarDetailsPanelT::setStarDetailsFromEvent, this, boost::placeholders::_1);
    EventBusT::getInstance()->subscribe(g);

    reset();
}

StarDetailsPanelT::~StarDetailsPanelT() {
}

void StarDetailsPanelT::reset() {

}

void StarDetailsPanelT::setStarDetailsFromEvent(const NewFrameEventT & newFrameEvent) {
    StarDetailsT starDetails;

    // TODO / HACK: Next step is to subscribe to a dedicated NewStarDetailsEvent...
    const std::shared_ptr<const ImageT>& resultImage = newFrameEvent.getResultImage();

    if (resultImage) {
        starDetails.setMin(resultImage->min());
        starDetails.setMax(resultImage->max());
    }

    this->setStarDetails(starDetails);
}

std::string StarDetailsPanelT::floatToString(float f) {
    std::string text;

    std::ostringstream oss;
    oss.precision(4);
    oss << f;
    text = oss.str();

    return text;
}

void StarDetailsPanelT::setStarDetails(const StarDetailsT &starDetails) {
    LOG(debug) << "StarDetailsPanelT::setStarDetails..." << std::endl;

    m_ui->lblMin->setText(QString::fromStdString(floatToString(starDetails.getMin())));
    m_ui->lblMax->setText(QString::fromStdString(floatToString(starDetails.getMax())));


//    // Set Fwhm text
//    std::string text = "-";
//
//    if (fwhm.valid()) {
//        std::ostringstream oss;
//        oss.precision(4);
//        oss << fwhm.getValue();
//        text = oss.str();
//    }
//
//    m_ui->lblFwhmValue->setText(QString::fromStdString(text));
}
