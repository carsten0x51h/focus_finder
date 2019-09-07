#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FWHM_VIEW_WIDGET_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FWHM_VIEW_WIDGET_H_

// std includes

// Qt includes
#include <QWidget>
#include <QPixmap>
#include <QLabel>

#include "../../common/include/fwhm.h"


/**
*  @brief
*/
class FwhmViewWidgetT : public QLabel
{
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
FwhmViewWidgetT(QWidget * parent);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FwhmViewWidgetT();

	void reset();
	void setFwhm(const FwhmT & fwhm);

	void paintEvent(QPaintEvent * event) override;

signals:

protected slots:

protected:

private:
	void drawCross(QPainter & p, const QPointF & center, const QColor & crossColor, int penWidth, int halfCrossLength);
	void drawCircle(QPainter & p, const QPointF & center, const QColor & circleColor, int penWidth, int radius);

	FwhmT mFwhm;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FWHM_VIEW_WIDGET_H_*/
