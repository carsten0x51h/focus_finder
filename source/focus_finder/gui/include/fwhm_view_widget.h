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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FWHM_VIEW_WIDGET_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FWHM_VIEW_WIDGET_H_ SOURCE_FOCUS_FINDER_GUI_INCLUDE_FWHM_VIEW_WIDGET_H_

// std includes

// Qt includes
#include <QWidget>
#include <QPixmap>
#include <QLabel>

#include "../../common/include/fwhm.h"


/**
*  @brief
*/
class FwhmViewWidgetT : public QLabel {
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
    FwhmViewWidgetT(QWidget *parent);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FwhmViewWidgetT();

    void reset();

    void setFwhm(const FwhmT &fwhm);

    void paintEvent(QPaintEvent *event) override;

signals:

protected slots:

protected:

private:
    static void drawCross(QPainter &p, const QPointF &center, const QColor &crossColor, int penWidth, int halfCrossLength);

    static void drawCircle(QPainter &p, const QPointF &center, const QColor &circleColor, int penWidth, int radius);

    FwhmT mFwhm;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FWHM_VIEW_WIDGET_H_*/
