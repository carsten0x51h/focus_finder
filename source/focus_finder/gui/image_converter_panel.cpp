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

#include "include/abstract_mapper_panel.h"
#include "include/image_converter_panel.h"
#include "include/linear_bw_stretch_mapper_panel.h"
#include "include/spline_curve_mapper_panel.h"

#include "../common/include/logging.h"
#include "../common/include/dummy_camera.h"
#include "../common/include/dummy_device_manager.h"
#include "../common/include/image_converter_16to8.h"
#include "../common/include/profile_manager.h"

#include "ui_image_converter_panel.h"

#include <QSizePolicy>
#include <QPushButton>

Q_DECLARE_SMART_POINTER_METATYPE (std::shared_ptr)

void ImageConverterPanelT::onNewImageReceivedSlot() {
	LOG(info)
	<< "ImageConverterPanelT::onNewImageReceivedSlot..." << std::endl;

	// Notify selected mapper panel that there is a new image.
	QComboBox * cbx = m_ui->cbxImageConverters;

	AbstractMapperPanelT * currentMapperPanel = getWidgetFromCbx(
			cbx->currentIndex());

	currentMapperPanel->notifyNewImage();
}

ImageConverterPanelT::ImageConverterPanelT(QWidget * parent,
		FocusFinderLogicT & ffl) :
		QWidget(parent), m_ui(new Ui::ImageConverterPanel), mFfl(ffl), mPrevIdx(
				0), mCameraDevice(nullptr) {
	// Setup UI
	m_ui->setupUi(this);

	// Register in ffl to get notified when new image is available
	connect(this, &ImageConverterPanelT::newImageReceivedSignal, this,
			&ImageConverterPanelT::onNewImageReceivedSlot);

	updateProfile();

	// Register at profile manager to get notified if selected profile / device changes...
	mFfl.getProfileManager()->registerActiveProfileChangedListener(
			std::bind(&ImageConverterPanelT::updateProfile, this));

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(100);
	sizePolicy.setVerticalStretch(100);

	m_ui->widget->setSizePolicy(sizePolicy);
	m_ui->layImageConverterArea->setAlignment(Qt::AlignTop);

	// Set possible selections
	QComboBox * cbx = m_ui->cbxImageConverters;

	auto linearBWStretchMapperPanel = new LinearBWStretchMapperPanelT(
			m_ui->widget, ffl, "LinearBWStretchMapperFunction", true);

	cbx->addItem(QString("Linear stretch"),
			QVariant::fromValue(linearBWStretchMapperPanel));

	connect(linearBWStretchMapperPanel,
			&LinearBWStretchMapperPanelT::valueChangedSignal, this,
			&ImageConverterPanelT::valueChangedSlot);

	auto splineCurveMapperPanel = new SplineCurveMapperPanelT(m_ui->widget, ffl,
			"SplineCurveMapperFunction", false);

	cbx->addItem(QString("Spline curve"),
			QVariant::fromValue(splineCurveMapperPanel));

	connect(splineCurveMapperPanel,
			&SplineCurveMapperPanelT::valueChangedSignal, this,
			&ImageConverterPanelT::valueChangedSlot);

	connect(cbx, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
			&ImageConverterPanelT::currentIndexChanged);
}

ImageConverterPanelT::~ImageConverterPanelT() {
}

void ImageConverterPanelT::updateProfile() {

	auto newCameraDevice = mFfl.getCurrentCamera();

	bool hasOldCamera = (mCameraDevice != nullptr);

	if (mCameraDevice == newCameraDevice) {
		return;
	}

	if (hasOldCamera) {
		// There was already an old device - unregister listener and register to the new one.
		mCameraDevice->unregisterExposureCycleFinishedListener(
				mExposureCycleFinishedConnection);
	}

	// Register to new device
	if (newCameraDevice) {
		mExposureCycleFinishedConnection =
				newCameraDevice->registerExposureCycleFinishedListener(
						[&](RectT<unsigned int> /*roiRect*/, std::shared_ptr<const ImageT> /*resultImage*/, bool lastExposure) {
							LOG(info) << "ImageConverterPanelT::new image set! -> emit!" << lastExposure << std::endl;
							emit newImageReceivedSignal();
						});
	}

	mCameraDevice = newCameraDevice;
}

void ImageConverterPanelT::valueChangedSlot() {
	LOG(debug)
	<< "ImageConverterPanelT::valueChangedSlot..." << std::endl;

	// Recalculate the LUT since value of current mappe function has changed.
	mFfl.getImageConverter16To8()->update();
	emit valueChangedSignal();
}

AbstractMapperPanelT * ImageConverterPanelT::getWidgetFromCbx(int idx) {
	QComboBox * cbx = m_ui->cbxImageConverters;
	QVariant var = QVariant::fromValue(cbx->itemData(idx));
	auto widgetPtr = qvariant_cast<AbstractMapperPanelT *>(var);

	return widgetPtr;
}

void ImageConverterPanelT::currentIndexChanged(int idx) {

	getWidgetFromCbx(mPrevIdx)->setVisible(false);

	AbstractMapperPanelT * currentMapperPanel = getWidgetFromCbx(idx);
	currentMapperPanel->setVisible(true);

	auto mapperFunction = currentMapperPanel->getMapperFunction();

	LOG(debug)
	<< "mapperFunction: " << mapperFunction << std::endl;

	mFfl.getImageConverter16To8()->setMapperFunction(mapperFunction);

	mPrevIdx = idx;

	emit valueChangedSignal();
}
