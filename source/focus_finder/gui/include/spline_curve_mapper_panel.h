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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_SPLINE_CURVE_MAPPER_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_SPLINE_CURVE_MAPPER_PANEL_H_ SOURCE_FOCUS_FINDER_GUI_INCLUDE_SPLINE_CURVE_MAPPER_PANEL_H_

// std includes
#include <memory>

// Qt includes
#include <QWidget>

#include "../../common/include/focus_finder_logic.h"
#include "../../common/include/logging.h"
#include "../../common/include/spline_curve_mapper_function.h"

#include "abstract_mapper_panel.h"

#include "../ui_spline_curve_mapper_panel.h"


namespace Ui {
    class SplineCurveMapperPanel;
}

class SplineCurveMapperPanelT : public AbstractMapperPanelT {
Q_OBJECT


public:
    /**
    *  @brief
    *    Constructor
    */
    SplineCurveMapperPanelT(QWidget *parent, FocusFinderLogicT &ffl, const std::string &functionMapperName,
                            bool visible);

    ~SplineCurveMapperPanelT() override;

    void notifyNewImage() override;

    [[nodiscard]] std::shared_ptr<MapperFunctionT> getMapperFunction() const override;


signals:

    void valueChangedSignal() override;

protected slots:
    //void onRoiClearedSlot();

protected:
    const QScopedPointer<Ui::SplineCurveMapperPanel> m_ui;


private:
    void update();

    FocusFinderLogicT &mFfl;
    std::shared_ptr<SplineCurveMapperFunctionT> mFunctionMapper;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_SPLINE_CURVE_MAPPER_PANEL_H_*/
