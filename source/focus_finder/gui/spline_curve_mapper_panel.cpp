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

#include "include/spline_curve_mapper_panel.h"

#include "../common/include/logging.h"

#include "ui_spline_curve_mapper_panel.h"

SplineCurveMapperPanelT::SplineCurveMapperPanelT(QWidget * parent, FocusFinderLogicT & ffl, const std::string & functionMapperName, bool visible) : AbstractMapperPanelT(parent),
	m_ui(new Ui::SplineCurveMapperPanel),
	mFfl(ffl)
{
    // Setup UI
    m_ui->setupUi(this);

    setVisible(visible);

	//connect(mSelectableImageWidget, & SelectableImageWidgetT::roiClearedSignal, this, & ImageViewerPanelT::onRoiClearedSlot);

    LOG(debug) << "SplineCurveMapperPanelT::SplineCurveMapperPanelT..." << functionMapperName << std::endl;

    mFunctionMapper = std::static_pointer_cast<SplineCurveMapperFunctionT> (ffl.getMapperFunctionByName(functionMapperName));
}



SplineCurveMapperPanelT::~SplineCurveMapperPanelT()
{
	//delete mSelectableImageWidget;
}

void SplineCurveMapperPanelT::notifyNewImage() {
	LOG(debug) << "SplineCurveMapperPanelT::notifyNewImage..." << std::endl;
}

std::shared_ptr<MapperFunctionT> SplineCurveMapperPanelT::getMapperFunction() const {
	return mFunctionMapper;
}


void SplineCurveMapperPanelT::update() {
}
