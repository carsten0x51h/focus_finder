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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_SELECTABLE_IMAGE_WIDGET_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_SELECTABLE_IMAGE_WIDGET_H_

#include <QLabel>
#include <QMenu>
#include <QPointF>
#include <QRect>
#include <QSize>
#include <QResizeEvent>

#include "image_viewer_mode.h"
#include "selectable_image_widget.h"

// See: https://forum.qt.io/topic/11912/selecting-a-part-of-image/6

class QScrollArea;

class SelectableImageWidgetT : public QLabel
{
Q_OBJECT

public:
	SelectableImageWidgetT(QWidget *parent = 0);
	~SelectableImageWidgetT();

	void setFrame(const QPixmap & pixmap);
	void setSubFrame(const QRect & roiRect, const QPixmap & pixmap);
    void clearSelection();

    bool isPoiSet() const;
    void setPoi(const QPointF & poi);
    void clearPoi();
    const QSize & getPoiWindowSize() const;
    void setPoiWindowSize(const QSize & poiWindowSize);

	ImageViewerModeT::TypeE getMode() const;
	void setMode(ImageViewerModeT::TypeE mode);

    void update();

    void zoomIn();
    void zoomOut();


protected:
	void paintEvent(QPaintEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void wheelEvent(QWheelEvent * event);

private:
    void zoomBy(float factor);
	bool hasImage() const;

	QPoint translateToBase(const QPoint & p) const;
	QRect translateToBase(const QRect & r) const;

	void handleMousePressedEventNavigation(QMouseEvent * event);
	void handleMousePressedEventRoiSelect(QMouseEvent * event);
	void handleMousePressedEventPoiSelect(QMouseEvent * event);

	void handleMouseMoveEventNavigation(QMouseEvent * event);
	void handleMouseMoveEventRoiSelect(QMouseEvent * event);
	void handleMouseReleaseEventNavigation(QMouseEvent * event);
	void handleMouseReleaseEventRoiSelect(QMouseEvent * event);

	bool selectionStarted;
	bool moveStarted;
	QPoint mMoveDelta;


	QRect mSelectionRect;

	QRect mSubFrameRect;
	QPixmap mSubFramePixmap;

	//QMenu contextMenu; // May be used for context menu on selection later on

	ImageViewerModeT::TypeE mImageViewerMode;

	/**
	 * "mPixmapRect" stores the original pixmap rect size - i.e. the original size of the image.
	 * For the test the rect is "x: 0, y: 0, w: 2500, h: 1500".
	 * This value is later used by the paintEvent() method.
	 */
	QRectF mPixmapRect;

	/**
	 * Position at first left click (in pan mode) or after last mouse move. (QPointF).
	 */
	QPointF mReferencePoint;

	/**
	 * Scale factor of pixmap (only as drawn) - original pixmap is not touched.
	 */
	qreal mScaleFactor;

	QScrollArea * mParentScrollArea;


	QPoint mLastMousePos;


	QPointF mPoi;
	QSize mPoiWindowSize;

signals:
	// TODO: Question is if QImage should be returned by image viewer... or just Rect roi and QPoint poi!?
	void roiSelectedSignal(const QRect & roiRect, const QImage & roiImage);
	void roiClearedSignal();
	void poiSelectedSignal(const QPoint & poi);


private slots:
	//void saveSlot();

};

#endif // SOURCE_FOCUS_FINDER_GUI_INCLUDE_SELECTABLE_IMAGE_WIDGET_H_
