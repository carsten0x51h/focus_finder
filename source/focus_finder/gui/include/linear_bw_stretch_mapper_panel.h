#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_LINEAR_BW_STRETCH_MAPPER_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_LINEAR_BW_STRETCH_MAPPER_PANEL_H_

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


class LinearBWStretchMapperPanelT : public AbstractMapperPanelT
{
Q_OBJECT


public:
    /**
    *  @brief
    *    Constructor
    */
	LinearBWStretchMapperPanelT(QWidget * parent, FocusFinderLogicT & ffl, const std::string & functionMapperName, bool visible);
	~LinearBWStretchMapperPanelT();

    std::shared_ptr<MapperFunctionT> getMapperFunction() const;
    void notifyNewImage();

	int getMaxPixelValue() const;
	void setMaxPixelValue(int maxPixelValue);

	int getBlackPoint() const;
	void setBlackPoint(int blackPoint);

	int getWhitePoint() const;
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

	FocusFinderLogicT & mFfl;
    std::shared_ptr<LinearBWStretchMapperFunctionT> mFunctionMapper;
    int mMaxPixelValue;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_LINEAR_BW_STRETCH_MAPPER_PANEL_H_*/
