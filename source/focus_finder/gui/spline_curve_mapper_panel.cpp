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
