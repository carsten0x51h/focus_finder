#include <QSizePolicy>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QKeyEvent>

#include "include/image_viewer_panel.h"

#include "../common/include/logging.h"
#include "../common/include/profile_manager.h"
#include "../common/include/focus_finder_profile.h"

#include "ui_image_viewer_panel.h"

ImageViewerPanelT::ImageViewerPanelT(FocusFinderLogicT & ffl) :
m_ui(new Ui::ImageViewerPanel),
mFfl(ffl),
selectionStarted(false),
moveStarted(false),
mMoveDelta(0, 0)
{
	// Setup UI
	m_ui->setupUi(this);

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(100);
	sizePolicy.setVerticalStretch(100);

	mSelectableImageWidget = new SelectableImageWidgetT(m_ui->scrollArea);
	mSelectableImageWidget->setSizePolicy(sizePolicy);

	m_ui->layScrollContainer->addWidget(mSelectableImageWidget, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);
	//m_ui->scrollArea->setWidget(mSelectableImageWidget);

	connect(mSelectableImageWidget, & SelectableImageWidgetT::roiClearedSignal, this, & ImageViewerPanelT::onRoiClearedSlot);
	connect(mSelectableImageWidget, & SelectableImageWidgetT::roiSelectedSignal, this, & ImageViewerPanelT::onRoiSelectedSlot);
	connect(mSelectableImageWidget, & SelectableImageWidgetT::poiSelectedSignal, this, & ImageViewerPanelT::onPoiSelectedSlot);

	updateProfile();

	// Register at profile manager to get notified if selected profile / device changes...
	mFfl.getProfileManager()->registerActiveProfileChangedListener(
			std::bind(&ImageViewerPanelT::updateProfile, this));
}

ImageViewerPanelT::~ImageViewerPanelT() {
	delete mSelectableImageWidget;
}

void ImageViewerPanelT::updateProfile() {
	LOG(debug)
	<< "ImageViewerPanelT::updateProfile..." << std::endl;

	auto activeProfile = mFfl.getProfileManager()->getActiveProfile();

	SizeT<unsigned int> starWindowSize = (
			activeProfile ?
					activeProfile->getStarWindowSize() :
					FocusFinderProfileT::defaults().getStarWindowSize());

	mSelectableImageWidget->setPoiWindowSize(
			QSize(std::get<0>(starWindowSize), std::get<1>(starWindowSize)));
}

void ImageViewerPanelT::resizeEvent(QResizeEvent *event) {
	LOG(debug)
	<< "ImageViewerPanelT::resizeEvent..." << std::endl;
	mSelectableImageWidget->update();

	Q_UNUSED(event);
}

void ImageViewerPanelT::update() {
	QWidget::update();
	mSelectableImageWidget->update();
}

void ImageViewerPanelT::setMode(ImageViewerModeT::TypeE mode) {
	mSelectableImageWidget->setMode(mode);
}

void ImageViewerPanelT::clearSelection() {
	mSelectableImageWidget->clearSelection();
}

void ImageViewerPanelT::keyPressEvent(QKeyEvent *ev) {
	LOG(debug)
	<< "You Pressed key: " << ev->key() << std::endl;

	switch (ev->key()) {
	case 43: // +
		mSelectableImageWidget->zoomIn();
		break;
	case 45: // -
		mSelectableImageWidget->zoomOut();
		break;
	default:
		// Ignore for now
		break;
	}
}

ImageViewerModeT::TypeE ImageViewerPanelT::getMode() const {
	return mSelectableImageWidget->getMode();
}

void ImageViewerPanelT::setFrame(const QImage & image) {
	LOG(debug)
	<< "ImageViewerPanelT::setFrame..." << image.width() << "x"
			<< image.height() << std::endl;

	mSelectableImageWidget->setFrame(QPixmap::fromImage(image));
}

void ImageViewerPanelT::setSubFrame(const QRect & roiRect,
		const QImage & image) {
	LOG(debug)
	<< "ImageViewerPanelT::setSubFrame..." << image.width() << "x"
			<< image.height() << std::endl;

	mSelectableImageWidget->setSubFrame(roiRect, QPixmap::fromImage(image));
}

bool ImageViewerPanelT::isPoiSet() const {
	return mSelectableImageWidget->isPoiSet();
}

void ImageViewerPanelT::setPoi(const QPointF & poi) {
	LOG(debug)
	<< "ImageViewerPanelT::setPoi..." << poi.x() << "x" << poi.y() << std::endl;
	mSelectableImageWidget->setPoi(poi);
}

void ImageViewerPanelT::clearPoi() {
	LOG(debug)
	<< "ImageViewerPanelT::clearPoi..." << std::endl;
	mSelectableImageWidget->clearPoi();
}

void ImageViewerPanelT::onRoiClearedSlot() {
	LOG(debug)
	<< "ImageViewerPanelT::onRoiClearedSlot()..." << std::endl;
	emit roiClearedSignal();
}

void ImageViewerPanelT::onRoiSelectedSlot(const QRect & roiRect,
		const QImage & roiImage) {
	LOG(debug)
	<< "ImageViewerPanelT::onRoiSelectedSlot()..." << std::endl;
	emit roiSelectedSignal(roiRect, roiImage);
}

void ImageViewerPanelT::onPoiSelectedSlot(const QPoint & poi) {
	LOG(debug)
	<< "ImageViewerPanelT::onPoiSelectedSlot()..." << std::endl;
	emit poiSelectedSignal(poi);
}
