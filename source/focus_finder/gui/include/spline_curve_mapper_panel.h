#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_SPLINE_CURVE_MAPPER_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_SPLINE_CURVE_MAPPER_PANEL_H_

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

class SplineCurveMapperPanelT : public AbstractMapperPanelT
{
Q_OBJECT


public:
    /**
    *  @brief
    *    Constructor
    */
	SplineCurveMapperPanelT(QWidget * parent, FocusFinderLogicT & ffl, const std::string & functionMapperName, bool visible);
	~SplineCurveMapperPanelT();

    void notifyNewImage();

	std::shared_ptr<MapperFunctionT> getMapperFunction() const;


signals:
	void valueChangedSignal();

protected slots:
	//void onRoiClearedSlot();

protected:
	const QScopedPointer<Ui::SplineCurveMapperPanel> m_ui;


private:
	void update();

	FocusFinderLogicT & mFfl;
    std::shared_ptr<SplineCurveMapperFunctionT> mFunctionMapper;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_SPLINE_CURVE_MAPPER_PANEL_H_*/
