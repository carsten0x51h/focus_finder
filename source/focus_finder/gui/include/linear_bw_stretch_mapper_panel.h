/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c [at] lost-infinity.com>
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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_LINEAR_BW_STRETCH_MAPPER_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_LINEAR_BW_STRETCH_MAPPER_PANEL_H_ SOURCE_FOCUS_FINDER_GUI_INCLUDE_LINEAR_BW_STRETCH_MAPPER_PANEL_H_

// std includes
#include <memory>

// Qt includes
#include <QWidget>

#include "../../common/include/focus_finder_logic.h"
#include "../../common/include/logging.h"
#include "../../common/include/linear_bw_stretch_mapper_function.h"

#include "abstract_mapper_panel.h"

#include "../ui_linear_bw_stretch_mapper_panel.h"


namespace Ui {
    class LinearBWStretchMapperPanel;
}


class LinearBWStretchMapperPanelT : public AbstractMapperPanelT {
Q_OBJECT


public:
    /**
    *  @brief
    *    Constructor
    */
    LinearBWStretchMapperPanelT(QWidget *parent, FocusFinderLogicT &ffl, const std::string &functionMapperName,
                                bool visible);

    ~LinearBWStretchMapperPanelT() override;

    [[nodiscard]] std::shared_ptr<MapperFunctionT> getMapperFunction() const override;

    void notifyNewImage() override;

    [[nodiscard]] int getMaxPixelValue() const;

    void setMaxPixelValue(int maxPixelValue);

    [[nodiscard]] int getBlackPoint() const;

    void setBlackPoint(int blackPoint);

    [[nodiscard]] int getWhitePoint() const;

    void setWhitePoint(int whitePoint);


protected slots:

    void onBlackPointSliderChangedSlot(int blackPoint);

    void onBlackPointSpinBoxChangedSlot(int blackPoint);

    void onWhitePointSliderChangedSlot(int whitePoint);

    void onWhitePointSpinBoxChangedSlot(int whitePoint);

    void on_btnFit_clicked();


protected:
    const QScopedPointer<Ui::LinearBWStretchMapperPanel> m_ui;


private:
    void innerSetBlackPoint(int blackPoint);

    void innerSetWhitePoint(int whitePoint);

    void setBlackPointChecked(int blackPoint);

    void setWhitePointChecked(int whitePoint);

    void setPointsChecked(int blackPoint, int whitePoint);

    void updateMapperFunction();

    void performFit();

    FocusFinderLogicT &mFfl;
    std::shared_ptr<LinearBWStretchMapperFunctionT> mFunctionMapper;
    int mMaxPixelValue;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_LINEAR_BW_STRETCH_MAPPER_PANEL_H_*/
