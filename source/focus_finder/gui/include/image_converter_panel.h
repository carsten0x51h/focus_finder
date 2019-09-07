#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_IMAGE_CONVERTER_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_IMAGE_CONVERTER_PANEL_H_

// std includes
#include <memory>

// Qt includes
#include <QWidget>

#include "../../common/include/focus_finder_logic.h"
#include "../../common/include/logging.h"

#include "../ui_image_converter_panel.h"


class AbstractMapperPanelT;
class CameraT;

namespace Ui {
    class ImageConverterPanel;
}

class ImageConverterPanelT : public QWidget
{
    Q_OBJECT


public:
    /**
    *  @brief
    *    Constructor
    */
	ImageConverterPanelT(QWidget * parent, FocusFinderLogicT & ffl);
	~ImageConverterPanelT();


signals:
	void valueChangedSignal();
	void newImageReceivedSignal();

protected slots:
	void currentIndexChanged(int idx);
	void valueChangedSlot();
	void onNewImageReceivedSlot();

protected:
	const QScopedPointer<Ui::ImageConverterPanel> m_ui;


private:
	AbstractMapperPanelT * getWidgetFromCbx(int idx);
	void updateProfile();

	FocusFinderLogicT & mFfl;
	int mPrevIdx;
	std::shared_ptr<CameraT> mCameraDevice;
	boost::signals2::connection mExposureCycleFinishedConnection;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_IMAGE_CONVERTER_PANEL_H_*/
