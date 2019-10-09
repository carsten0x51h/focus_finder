#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_WIDGET_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_WIDGET_H_

// std includes

// Qt includes
#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QPaintEvent>

#include "../../common/include/focus_curve.h"
#include "../../common/include/image.h"

class FocusCurveRecordT;

/**
*  @brief
*/
class FocusCurveViewWidgetT : public QLabel
{
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
	FocusCurveViewWidgetT(QWidget * parent);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FocusCurveViewWidgetT();

	void reset();
	void setFocusCurve(const FocusCurveT & focusCurve);
  void addFocusCurveRecord(std::shared_ptr<FocusCurveRecordT> focusCurveRecord);

	void paintEvent(QPaintEvent * event) override;

signals:

protected slots:

protected:

private:
	FocusCurveT mFocusCurve;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_WIDGET_H_*/
