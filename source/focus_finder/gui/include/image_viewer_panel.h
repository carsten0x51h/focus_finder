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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_IMAGE_VIEWER_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_IMAGE_VIEWER_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QImage>
#include <QGraphicsRectItem>
#include <QRect>
#include <QLabel>

#include "../../common/include/focus_finder_logic.h"
#include "../../common/include/logging.h"

#include "selectable_image_widget.h"
#include "image_viewer_mode.h"


namespace Ui {
    class ImageViewerPanel;
}

class DeviceInterfaceT;


class ImageViewerPanelT : public QWidget
{
    Q_OBJECT


public:
    /**
    *  @brief
    *    Constructor
    */
	ImageViewerPanelT(FocusFinderLogicT & ffl);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~ImageViewerPanelT();

    void setFrame(const QImage & image);
    void setSubFrame(const QRect & roiRect, const QImage & image); // TODO: Rename to setRoi?! Or rename setFrame() to setImage() amd rename setSubFrame() to setFrame()...

    bool isPoiSet() const;
    void setPoi(const QPointF & poi);
    void clearPoi();

    void update();

    void clearSelection();

    void setMode(ImageViewerModeT::TypeE mode);
    ImageViewerModeT::TypeE getMode() const;


signals:
	void roiClearedSignal();
	void roiSelectedSignal(const QRect & roiRect, const QImage & roiImage);
	void poiSelectedSignal(const QPoint & poi);

protected slots:
	void onRoiClearedSlot();
	void onRoiSelectedSlot(const QRect & roiRect, const QImage & roiImage);
	void onPoiSelectedSlot(const QPoint & poi);

protected:
	const QScopedPointer<Ui::ImageViewerPanel> m_ui;

	void resizeEvent(QResizeEvent *event) override;
	void keyPressEvent(QKeyEvent *ev) override;


private:
	void updateProfile();

	FocusFinderLogicT & mFfl;

	SelectableImageWidgetT * mSelectableImageWidget;

	bool selectionStarted;
	bool moveStarted;
	QPoint mMoveDelta;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_IMAGE_VIEWER_PANEL_H_*/
