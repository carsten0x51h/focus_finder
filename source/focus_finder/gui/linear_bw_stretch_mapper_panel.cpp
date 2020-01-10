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

#include "include/linear_bw_stretch_mapper_panel.h"

#include "../common/include/logging.h"

#include "ui_linear_bw_stretch_mapper_panel.h"

LinearBWStretchMapperPanelT::LinearBWStretchMapperPanelT(QWidget * parent,
		FocusFinderLogicT & ffl, const std::string & functionMapperName,
		bool visible) :
		AbstractMapperPanelT(parent), m_ui(new Ui::LinearBWStretchMapperPanel), mFfl(
				ffl), mMaxPixelValue(65535) {
	// Setup UI
	m_ui->setupUi(this);

	setVisible(visible);

	mFunctionMapper = std::static_pointer_cast<LinearBWStretchMapperFunctionT>(
			ffl.getMapperFunctionByName(functionMapperName));

	int singleStepSize = 1;
	int pageStepSize = mMaxPixelValue / 100;

	m_ui->sliderBlackPoint->setMinimum(0);
	m_ui->sliderBlackPoint->setMaximum(mMaxPixelValue);
	m_ui->sliderBlackPoint->setSingleStep(singleStepSize);
	m_ui->sliderBlackPoint->setPageStep(pageStepSize);

	m_ui->spinBoxBlackPoint->setMinimum(0);
	m_ui->spinBoxBlackPoint->setMaximum(mMaxPixelValue);
	m_ui->spinBoxBlackPoint->setSingleStep(singleStepSize);

	m_ui->sliderWhitePoint->setMinimum(0);
	m_ui->sliderWhitePoint->setMaximum(mMaxPixelValue);
	m_ui->sliderWhitePoint->setSingleStep(singleStepSize);
	m_ui->sliderWhitePoint->setPageStep(pageStepSize);

	m_ui->spinBoxWhitePoint->setMinimum(0);
	m_ui->spinBoxWhitePoint->setMaximum(mMaxPixelValue);
	m_ui->spinBoxWhitePoint->setSingleStep(singleStepSize);

	connect(m_ui->sliderBlackPoint, &QSlider::valueChanged, this,
			&LinearBWStretchMapperPanelT::onBlackPointSliderChangedSlot);
	connect(m_ui->sliderWhitePoint, &QSlider::valueChanged, this,
			&LinearBWStretchMapperPanelT::onWhitePointSliderChangedSlot);

	connect(m_ui->spinBoxBlackPoint,
			QOverload<int>::of(&QSpinBox::valueChanged), this,
			&LinearBWStretchMapperPanelT::onBlackPointSpinBoxChangedSlot);
	connect(m_ui->spinBoxWhitePoint,
			QOverload<int>::of(&QSpinBox::valueChanged), this,
			&LinearBWStretchMapperPanelT::onWhitePointSpinBoxChangedSlot);
}

LinearBWStretchMapperPanelT::~LinearBWStretchMapperPanelT() {
}

void LinearBWStretchMapperPanelT::performFit() {
	const FrameT & frame = mFfl.getLastFrame();
	auto img = frame.getImage();

	if (img != nullptr) {
		uint16_t max;
		uint16_t min = img->min_max(max);

		LOG(debug)
		<< "LinearBWStretchMapperPanelT::performFit...min=" << min << ", max="
				<< max << std::endl;

		setPointsChecked(min, max);
	}
}

void LinearBWStretchMapperPanelT::notifyNewImage() {
	LOG(debug)
	<< "LinearBWStretchMapperPanelT::notifyNewImage..." << std::endl;

	if (m_ui->chkAutoFit->isChecked()) {
		performFit();
	}
}

void LinearBWStretchMapperPanelT::on_btnFit_clicked() {
	LOG(debug)
	<< "AutoFit pressed..." << std::endl;
	performFit();
}

int LinearBWStretchMapperPanelT::getMaxPixelValue() const {
	return mMaxPixelValue;
}

void LinearBWStretchMapperPanelT::setMaxPixelValue(int maxPixelValue) {
	mMaxPixelValue = maxPixelValue;
}

// Internal function
void LinearBWStretchMapperPanelT::innerSetBlackPoint(int blackPoint) {
	m_ui->sliderBlackPoint->blockSignals(true);
	m_ui->sliderBlackPoint->setValue(blackPoint);
	m_ui->sliderBlackPoint->blockSignals(false);

	m_ui->spinBoxBlackPoint->blockSignals(true);
	m_ui->spinBoxBlackPoint->setValue(blackPoint);
	m_ui->spinBoxBlackPoint->blockSignals(false);
}

// Internal function
void LinearBWStretchMapperPanelT::innerSetWhitePoint(int whitePoint) {
	m_ui->sliderWhitePoint->blockSignals(true);
	m_ui->sliderWhitePoint->setValue(whitePoint);
	m_ui->sliderWhitePoint->blockSignals(false);

	m_ui->spinBoxWhitePoint->blockSignals(true);
	m_ui->spinBoxWhitePoint->setValue(whitePoint);
	m_ui->spinBoxWhitePoint->blockSignals(false);
}

int LinearBWStretchMapperPanelT::getBlackPoint() const {
	return m_ui->sliderBlackPoint->value();
}

void LinearBWStretchMapperPanelT::setBlackPoint(int blackPoint) {
	setBlackPointChecked(blackPoint);
}

int LinearBWStretchMapperPanelT::getWhitePoint() const {
	return m_ui->sliderWhitePoint->value();
}

void LinearBWStretchMapperPanelT::setWhitePoint(int whitePoint) {
	setWhitePointChecked(whitePoint);
}

void LinearBWStretchMapperPanelT::setPointsChecked(int blackPoint,
		int whitePoint) {

	LOG(debug)
	<< "LinearBWStretchMapperPanelT::setPointsChecked... black=" << blackPoint
			<< ", white=" << whitePoint << std::endl;

	// TODO: better throw?
	if (whitePoint < blackPoint) {
		whitePoint = blackPoint;
	}

	innerSetWhitePoint(whitePoint);
	innerSetBlackPoint(blackPoint);

	updateMapperFunction();
}

void LinearBWStretchMapperPanelT::setBlackPointChecked(int blackPoint) {

	int whitePoint = m_ui->sliderWhitePoint->value();

	if (blackPoint > whitePoint) {
		blackPoint = whitePoint;
	}

	innerSetBlackPoint(blackPoint);

	updateMapperFunction();
}

void LinearBWStretchMapperPanelT::setWhitePointChecked(int whitePoint) {

	int blackPoint = m_ui->sliderBlackPoint->value();

	if (whitePoint < blackPoint) {
		whitePoint = blackPoint;
	}

	innerSetWhitePoint(whitePoint);

	updateMapperFunction();
}

void LinearBWStretchMapperPanelT::onBlackPointSliderChangedSlot(
		int blackPoint) {
	setBlackPointChecked(blackPoint);
}

void LinearBWStretchMapperPanelT::onBlackPointSpinBoxChangedSlot(
		int blackPoint) {
	setBlackPointChecked(blackPoint);
}

void LinearBWStretchMapperPanelT::onWhitePointSliderChangedSlot(
		int whitePoint) {
	setWhitePointChecked(whitePoint);
}

void LinearBWStretchMapperPanelT::onWhitePointSpinBoxChangedSlot(
		int whitePoint) {
	setWhitePointChecked(whitePoint);
}

std::shared_ptr<MapperFunctionT> LinearBWStretchMapperPanelT::getMapperFunction() const {
	return mFunctionMapper;
}

void LinearBWStretchMapperPanelT::updateMapperFunction() {

	int blackPoint = m_ui->sliderBlackPoint->value();
	int whitePoint = m_ui->sliderWhitePoint->value();

	LOG(debug)
	<< "LinearBWStretchMapperPanelT::update - Black: " << blackPoint
			<< ", white: " << whitePoint << std::endl;

	mFunctionMapper->setBlackPoint(blackPoint);
	mFunctionMapper->setWhitePoint(whitePoint);

	// MainWindow can register on this signal ... and call updateImage() then...
	emit valueChangedSignal();
}
