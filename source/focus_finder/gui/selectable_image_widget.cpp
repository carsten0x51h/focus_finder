#include "include/selectable_image_widget.h"

#include "../common/include/logging.h"

// Qt includes
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QDebug>
#include <QAction>
#include <QFileDialog>
#include <QtMath>
#include <QScrollArea>
#include <QScrollBar>
#include <QLayout>
#include <QCursor>

// std includes
#include <cmath>

SelectableImageWidgetT::SelectableImageWidgetT(QWidget *parent) :
		QLabel(parent), selectionStarted(false), moveStarted(false), mMoveDelta(
				0, 0), mImageViewerMode(ImageViewerModeT::NAVIGATION), mScaleFactor(
				1.0), mPixmapRect(0, 0, 0, 0), mSubFrameRect(0, 0, 0, 0), mPoi(
				-1.0f, -1.0f), mPoiWindowSize(32, 32) {
	mParentScrollArea = static_cast<QScrollArea*>(parent);

	//QAction *saveAction=contextMenu.addAction("Save");
	//connect(saveAction,SIGNAL(triggered()),this,SLOT(saveSlot()));
}

SelectableImageWidgetT::~SelectableImageWidgetT() {
}

void SelectableImageWidgetT::clearSelection() {
	mSelectionRect.setHeight(0);
	mSelectionRect.setWidth(0);

	update();

	emit roiClearedSignal();
}

bool SelectableImageWidgetT::hasImage() const {
	return (pixmap() != nullptr && !pixmap()->isNull());
}

ImageViewerModeT::TypeE SelectableImageWidgetT::getMode() const {
	return mImageViewerMode;
}

QPoint SelectableImageWidgetT::translateToBase(const QPoint & p) const {
	return p * (1.0 / mScaleFactor);
	//return (p - mLastMousePos) * (1.0 / mScaleFactor);
	//return (p - rect().center()) * (1.0 / mScaleFactor);
}

QRect SelectableImageWidgetT::translateToBase(const QRect & r) const {
	QRect res(r);
	const float inverseScaleFactor = 1.0 / mScaleFactor;

	res.setTopLeft((r.topLeft()) * inverseScaleFactor);
	//res.setTopLeft((r.topLeft() - mLastMousePos) * inverseScaleFactor);
	//res.setTopLeft((r.topLeft() - rect().center()) * inverseScaleFactor);

	// Also adjust width and height
	res.setWidth(r.width() * inverseScaleFactor);
	res.setHeight(r.height() * inverseScaleFactor);

	LOG(debug) << "SelectableImageWidgetT::translateToBase..."
			<< "IN rect: [x=" << r.x() << ", y=" << r.y() << ", w=" << r.width()
			<< ", h=" << r.height() << "] ->" << "OUT rect: [x=" << res.x()
			<< ", y=" << res.y() << ", w=" << res.width() << ", h="
			<< res.height() << "]." << std::endl;

	return res;
}

void SelectableImageWidgetT::setMode(ImageViewerModeT::TypeE mode) {
	mImageViewerMode = mode;
}

void SelectableImageWidgetT::paintEvent(QPaintEvent * event) {
	if (hasImage()) {
		LOG(debug)
				<< "SelectableImageWidgetT::handlePaintEventNaviagtion..."
				<< std::endl;

		QPainter p(this);

		p.scale(mScaleFactor, mScaleFactor);

		// Draw the sub frame (if any)
		if (mSubFrameRect.width() > 0 && mSubFrameRect.height() > 0) {
			// Paint rest of the pixmap black
			QPixmap bgPixmap(pixmap()->size());
			bgPixmap.fill(Qt::black);
			p.drawPixmap(mPixmapRect.topLeft() /*top left of pixmap*/,
					bgPixmap);

			// Then paint only the sub frame
			p.drawPixmap(mSubFrameRect.topLeft() /*top left of pixmap*/,
					mSubFramePixmap);
		} else {
			// Simply draw the complete pixmap
			p.drawPixmap(mPixmapRect.topLeft() /*top left of pixmap*/,
					*pixmap());
		}

		// Draw rect onto the pixmap - for now the outer 2 pixels of the image are overdrawn.
		const unsigned int pixmapBorderSize = 4;
		p.setPen(
				QPen(QBrush(QColor(0, 255, 0, 255)), pixmapBorderSize,
						Qt::SolidLine));
		p.setBrush(QBrush(QColor(255, 255, 255, 0)));
		p.drawRect(mPixmapRect);

		// Draw the selection rectagle
		p.setPen(QPen(QBrush(QColor(255, 0, 0, 180)), 2, Qt::SolidLine));
		p.setBrush(QBrush(QColor(255, 255, 255, 0)));
		p.drawRect(mSelectionRect);

		// Draw the poi selection (if any)
		if (mPoi.x() > 0 && mPoi.y() > 0) {

			// Draw cross
			const float lineLengthPx = 0.9
					* std::min(mPoiWindowSize.width(), mPoiWindowSize.height());

			//TODO: Debug line coordinates - does it actually allow floats??? Probably not...!?
			p.setPen(QPen(QBrush(QColor(255, 255, 0, 180)), 1, Qt::SolidLine));

			//inline void drawLine(const QPointF &p1, const QPointF &p2);
			// mPoi is float...
			// lineLengthPx is float
			QPointF p1(mPoi.x() - lineLengthPx, mPoi.y());
			QPointF p2(mPoi.x() + lineLengthPx, mPoi.y());

			LOG(debug)
					<< "SelectableImageWidgetT::paintEvent - draw cross... p1=("
					<< p1.x() << ", " << p1.y() << "), p2=(" << p2.x() << ", "
					<< p2.y() << ")..." << std::endl;

			p.drawLine(p1, p2);

			QPointF p3(mPoi.x(), mPoi.y() - lineLengthPx);
			QPointF p4(mPoi.x(), mPoi.y() + lineLengthPx);

			LOG(debug)
					<< "SelectableImageWidgetT::paintEvent - draw cross... p3=("
					<< p3.x() << ", " << p3.y() << "), p4=(" << p4.x() << ", "
					<< p4.y() << ")..." << std::endl;

			p.drawLine(p3, p4);

			// Draw rectangle
			p.setPen(QPen(QBrush(QColor(0, 0, 255, 180)), 1, Qt::SolidLine));

			const float windowWidth = mPoiWindowSize.width();
			const float windowHeight = mPoiWindowSize.height();
			const float halfWidth = windowWidth / 2.0;
			const float halfHeight = windowHeight / 2.0;

			p.drawRect(QRectF(mPoi.x() - halfWidth, mPoi.y() - halfHeight, windowWidth, windowHeight));
		}
	}
	Q_UNUSED(event);
}

void SelectableImageWidgetT::handleMousePressedEventNavigation(
		QMouseEvent * event) {
	if (mImageViewerMode == ImageViewerModeT::NAVIGATION && hasImage()) {
		mReferencePoint = event->globalPos();
		//qApp->setOverrideCursor(Qt::ClosedHandCursor);
		setMouseTracking(true);
	}
}

void SelectableImageWidgetT::handleMousePressedEventRoiSelect(
		QMouseEvent * event) {
	if (mImageViewerMode == ImageViewerModeT::ROI_SELECT && hasImage()) {
		LOG(debug) << "SelectableImageWidgetT::handlePaintEventRoiSelect..."
				<< std::endl;

		QPoint posBase = translateToBase(event->pos());

		if (mSelectionRect.contains(posBase)) {
			// TODO: Shift only when clicking and holding left mouse button, right click -> ctx menu?
			LOG(debug) << "MOVE started - x: " << posBase.x() << ", y: "
					<< posBase.y() << std::endl;

			moveStarted = true;

			mMoveDelta.setX(posBase.x() - mSelectionRect.x());
			mMoveDelta.setY(posBase.y() - mSelectionRect.y());
		} else {
			if (event->button() == Qt::LeftButton) {
				selectionStarted = true;
				mSelectionRect.setTopLeft(event->pos());
				mSelectionRect.setBottomRight(event->pos());

				mSelectionRect = translateToBase(mSelectionRect);

			} else if (event->button() == Qt::RightButton) {
				selectionStarted = false;
				clearSelection();
			} else {
				// Do nothing at the moment...
			}
		}
	}
}

void SelectableImageWidgetT::handleMousePressedEventPoiSelect(
		QMouseEvent * event) {
	if (mImageViewerMode == ImageViewerModeT::POI_SELECT && hasImage()) {
		LOG(debug)
				<< "SelectableImageWidgetT::handleMousePressedEventPoiSelect... position = ("
				<< event->pos().x() << ", " << event->pos().y() << ")..."
				<< std::endl;

		// The mouse coordinate "includes" the zoom and first needs to be transformed back to "pixmap" coordinates.
		QPoint selectedPoi = translateToBase(event->pos());

		// Need to update the reference point because in case the user
		// does not release the left mouse button, it is not set to the current
		// position and the image would jump to a different location.
		mReferencePoint = event->globalPos();

		emit poiSelectedSignal(selectedPoi);
	}
}

void SelectableImageWidgetT::mousePressEvent(QMouseEvent * event) {
	handleMousePressedEventNavigation(event);
	handleMousePressedEventRoiSelect(event);
	handleMousePressedEventPoiSelect(event);
}

void SelectableImageWidgetT::handleMouseMoveEventNavigation(
		QMouseEvent * event) {

	if (mImageViewerMode == ImageViewerModeT::NAVIGATION && hasImage()) {

		QPointF d = (event->globalPos() - mReferencePoint);

		mReferencePoint = event->globalPos();

		LOG(debug) << "delta - x: " << d.x() << ", y: " << d.y()
				<< ", HorzScrollBar: "
				<< mParentScrollArea->horizontalScrollBar()->value()
				<< ", VertScrollBar: "
				<< mParentScrollArea->verticalScrollBar()->value() << std::endl;

		int hMax = mParentScrollArea->horizontalScrollBar()->maximum();
		int vMax = mParentScrollArea->verticalScrollBar()->maximum();

		LOG(debug) << "vMax: " << vMax << ", hMax: " << hMax << std::endl;

		if (hMax > 0) {
			int newPosX = mParentScrollArea->horizontalScrollBar()->value()
					- d.x();
			mParentScrollArea->horizontalScrollBar()->setValue(newPosX);
			LOG(debug) << "newPosX: " << newPosX << std::endl;
		} else {
			// Currently we do not allow moving the image if it is smaller than the viewport
		}

		if (vMax > 0) {
			int newPosY = mParentScrollArea->verticalScrollBar()->value()
					- d.y();
			mParentScrollArea->verticalScrollBar()->setValue(newPosY);
			LOG(debug) << "newPosY: " << newPosY << std::endl;
		} else {
			// Currently we do not allow moving the image if it is smaller than the viewport
		}

		update();
	}
}

void SelectableImageWidgetT::handleMouseMoveEventRoiSelect(
		QMouseEvent * event) {
	QPoint posBase = translateToBase(event->pos());

	if (mImageViewerMode == ImageViewerModeT::ROI_SELECT && hasImage()) {
		if (selectionStarted) {
			mSelectionRect.setBottomRight(posBase);
		} else if (moveStarted) {
			mSelectionRect.moveTo(posBase - mMoveDelta);
		}
		repaint();
	}
}

void SelectableImageWidgetT::mouseMoveEvent(QMouseEvent * event) {
	mLastMousePos = event->pos(); // NOTE: Requires mouse tracking

	LOG(debug) << "mLastMousePos - x: " << mLastMousePos.x() << ", y: "
			<< mLastMousePos.y() << std::endl;

	handleMouseMoveEventNavigation(event);
	handleMouseMoveEventRoiSelect(event);
}

void SelectableImageWidgetT::handleMouseReleaseEventNavigation(
		QMouseEvent * event) {
	if (mImageViewerMode == ImageViewerModeT::NAVIGATION && hasImage()) {
		//qApp->restoreOverrideCursor();

		setMouseTracking(false);
		Q_UNUSED(event);
	}
}

void SelectableImageWidgetT::handleMouseReleaseEventRoiSelect(
		QMouseEvent * event) {
	if (mImageViewerMode == ImageViewerModeT::ROI_SELECT) {
		selectionStarted = false;
		moveStarted = false;

		// Signal that ROI was selcted / cleared
		QRect normalizedSelectionRect = mSelectionRect.normalized();

		if (normalizedSelectionRect.width() > 0
				&& normalizedSelectionRect.height() > 0) {
			emit roiSelectedSignal(normalizedSelectionRect,
					this->pixmap()->copy(normalizedSelectionRect).toImage());
		} else {
			emit roiClearedSignal();
		}
		repaint();
	}
	Q_UNUSED(event);
}

void SelectableImageWidgetT::mouseReleaseEvent(QMouseEvent * event) {
	handleMouseReleaseEventNavigation(event);
	handleMouseReleaseEventRoiSelect(event);
}

void SelectableImageWidgetT::wheelEvent(QWheelEvent * event) {
	if (event->orientation() == Qt::Vertical) {
		mLastMousePos = event->pos();

		double angle = event->angleDelta().y();
		double factor = qPow(1.0015, angle);

		LOG(debug) << "angle: " << angle << ", factor: " << factor
				<< std::endl;

		zoomBy(factor);

		// This call avoids propagation of mouse wheel events to parent scroll area
		// which would then move the scroll bars.
		event->accept();
	}
}

void SelectableImageWidgetT::zoomBy(float factor) {

	if (!hasImage()) {
		return;
	}

	if ((mScaleFactor > 180 && factor > 1.0)
			|| (mScaleFactor < 0.3 && factor < 1.0)) {
		return;
	}

	LOG(debug) << "mLastMousePos - x: " << mLastMousePos.x() << ", y: "
			<< mLastMousePos.y() << std::endl;

	/**
	 * Pixel in x-direction will be shifted according to "factor".
	 * In case of zooming in, the position of x' will be bigger than x before: x' = factor * x
	 *
	 * We want the same pixel to be visible under the given mouse position "mLastMousePos".
	 * Therefore we need to shoft the pixmap to the right (or the scroll bar to the left).
	 * The given mouse position "mLastMousePos" is already in the coordinates of the pixmap.
	 *
	 * How?
	 *
	 * 1.) Calc. the new position of the pixel which was previously under the mouse using factor (x'= factor * x)
	 * 2.) Calculate the distance d = x' - x
	 * 3.) Apply d to the scroll bar...
	 */
	QPointF newPixelPos = factor * mLastMousePos;

	LOG(debug) << "mLastMousePos - x: " << mLastMousePos.x() << ", y: "
			<< mLastMousePos.y() << std::endl;
	LOG(debug) << "newPixelPos - x: " << newPixelPos.x() << ", y: "
			<< newPixelPos.y() << std::endl;

	QPointF dist = newPixelPos - mLastMousePos;

	mScaleFactor *= factor;

	LOG(debug) << "Total mScaleFactor: " << mScaleFactor << std::endl;

	update(); // TODO: here?! or later?

	int newPosX = mParentScrollArea->horizontalScrollBar()->value() + dist.x();
	int newPosY = mParentScrollArea->verticalScrollBar()->value() + dist.y();

	LOG(debug) << "horzValue: "
			<< mParentScrollArea->horizontalScrollBar()->value() << " (max="
			<< mParentScrollArea->horizontalScrollBar()->maximum() << ")"
			<< ", dist.x(): " << dist.x() << " -> newPosX: " << newPosX
			<< std::endl;

	LOG(debug) << "vertValue: "
			<< mParentScrollArea->verticalScrollBar()->value() << " (max="
			<< mParentScrollArea->verticalScrollBar()->maximum() << ")"
			<< ", dist.y(): " << dist.y() << " -> newPosY: " << newPosY
			<< std::endl;

	mParentScrollArea->horizontalScrollBar()->setValue(newPosX);
	mParentScrollArea->verticalScrollBar()->setValue(newPosY);
}

void SelectableImageWidgetT::zoomIn() {
	mLastMousePos = this->mapFromGlobal(QCursor::pos());
	LOG(debug) << "mLastMousePos - x: " << mLastMousePos.x() << ", y: "
			<< mLastMousePos.y() << std::endl;

	double factor = qPow(1.0015, 120);
	zoomBy(factor);
}

void SelectableImageWidgetT::zoomOut() {
	mLastMousePos = this->mapFromGlobal(QCursor::pos());
	LOG(debug) << "mLastMousePos - x: " << mLastMousePos.x() << ", y: "
			<< mLastMousePos.y() << std::endl;

	double factor = qPow(1.0015, -120);
	zoomBy(factor);
}

void SelectableImageWidgetT::update() {
	QLabel::update();

	if (hasImage()) {
		LOG(debug) << "Pixmap size: " << this->pixmap()->width() << "x"
				<< this->pixmap()->height() << std::endl;
		LOG(debug) << "Viewport size: "
				<< mParentScrollArea->viewport()->width() << "x"
				<< mParentScrollArea->viewport()->height() << std::endl;
		LOG(debug) << "Lbl size: " << this->size().width() << "x"
				<< this->size().height() << std::endl;

		// NOTE: Need to subtract width of left and right margin
		QMargins margins =
				mParentScrollArea->widget()->layout()->contentsMargins();

		int viewportWidth = mParentScrollArea->viewport()->width()
				- margins.left() - margins.right();
		int viewportHeight = mParentScrollArea->viewport()->height()
				- margins.top() - margins.bottom();

		int lblWidth = this->pixmap()->width() * mScaleFactor;
		int lblHeight = this->pixmap()->height() * mScaleFactor;

		LOG(debug) << "New lbl size: " << lblWidth << "x" << lblHeight
				<< std::endl;

		int newWidth = lblWidth < viewportWidth ? viewportWidth : lblWidth;
		int newHeight = lblHeight < viewportHeight ? viewportHeight : lblHeight;

		this->setFixedWidth(newWidth);
		this->setFixedHeight(newHeight);
	}
}

void SelectableImageWidgetT::setFrame(const QPixmap &pix) {
	LOG(debug) << "SelectableImageWidgetT::setFrame..." << std::endl;

	// Clear subFrame rect
	mSubFrameRect = QRect(0, 0, 0, 0);

	QLabel::setPixmap(pix);

	mPixmapRect = pixmap()->rect();

	LOG(debug) << "SelectableImageWidgetT::setPixmap - mPixmapRect - x: "
			<< mPixmapRect.x() << ", y: " << mPixmapRect.y() << ", w: "
			<< mPixmapRect.width() << ", h: " << mPixmapRect.height()
			<< std::endl;

	update();
}

void SelectableImageWidgetT::setSubFrame(const QRect & roiRect,
		const QPixmap & pixmap) {
	LOG(debug) << "SelectableImageWidgetT::setFrame..." << std::endl;

	// Only draw the subframe if roiRect is completely within mPixmapRect. Otherwise just print a warning.
	if (mPixmapRect.contains(roiRect)) {

		mSubFramePixmap = pixmap;
		mSubFrameRect = roiRect;

		update();

	} else {
		LOG(warning) << "Subframe coordinates " << "[" << roiRect.x()
				<< ", " << roiRect.y() << ", " << roiRect.width() << ", "
				<< roiRect.height() << "]"
				<< " are outside the frame coordinates " << "["
				<< mPixmapRect.x() << ", " << mPixmapRect.y() << ", "
				<< mPixmapRect.width() << ", " << mPixmapRect.height() << "]."
				<< " Not setting subframe." << std::endl;
	}
}

bool SelectableImageWidgetT::isPoiSet() const {
	// TODO: Not nice but... ok...
	return (mPoi.x() != -1 && mPoi.y() != -1);
}

void SelectableImageWidgetT::setPoi(const QPointF & poi) {
	mPoi = poi;
	update();
}

void SelectableImageWidgetT::clearPoi() {
	mPoi = QPoint(-1, -1);
	update();
}

const QSize & SelectableImageWidgetT::getPoiWindowSize() const {
	return mPoiWindowSize;
}

void SelectableImageWidgetT::setPoiWindowSize(const QSize & poiWindowSize) {
	mPoiWindowSize = poiWindowSize;
	update();
}

//void SelectableImageWidgetT::saveSlot()
//{
//QString fileName = QFileDialog::getSaveFileName(this, QObject::tr("Save File"),
//"/home",
//QObject::tr("Images (*.jpg)"));
//this->pixmap()->copy(mSelectionRect).save(fileName);
//}
