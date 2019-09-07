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

class DeviceT;


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
