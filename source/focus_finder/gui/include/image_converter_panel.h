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
