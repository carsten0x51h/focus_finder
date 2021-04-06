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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_WIDGET_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_WIDGET_H_ SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_WIDGET_H_

// std includes
#include <memory>

// Qt includes
#include <QPixmap>
#include <QLabel>
#include <QPoint>
#include <QPaintEvent>
#include <QColor>

#include "../../common/include/image.h"
#include "../../common/include/focus_measure_type.h"
#include "../../common/include/focus_finder_profile.h"
#include "../../common/include/point.h"

class FocusCurveRecordT;

class FocusCurveRecordSetT;

class FocusCurveRecorderT;

class FocusCurveRecorderLogicT;

class FocusCurveT;

/**
*  @brief
*/
class FocusCurveViewWidgetT : public QLabel {
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
    FocusCurveViewWidgetT(QWidget *parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic);

    /**
    *  @brief
    *    Destructor
    */
    ~FocusCurveViewWidgetT() override;

    void reset();

    //void setFocusCurve(std::shared_ptr<const FocusCurveT> focusCurve);
    void update();

    void drawCurveHack(std::shared_ptr<FocusCurveT> focusCurve);

    // FocusMeasureTypeT::TypeE getFocusMeasureType() const;
    // void setFocusMeasureType(FocusMeasureTypeT::TypeE focusMeasureType);

    void paintEvent(QPaintEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;


signals:

protected slots:

protected:

private:
    void drawFocusCurveRecordSet(QPainter *p, const std::vector<PointFT> &dataPoints,
                                 const FocusFinderProfileT &focusFinderProfile, const QColor& color);

    void drawFocusCurveRecordSets(QPainter *p);

    QPointF transformToScreenCoordinates(float focusPos, float focusMeasure);

    std::shared_ptr<FocusCurveRecorderLogicT> mFocusCurveRecorderLogic;

    QPoint mLastMousePos;
    //  FocusMeasureTypeT::TypeE mFocusMeasureType;

    std::shared_ptr<FocusCurveT> mFocusCurveHack;

    // HACK! Remove! Get values ... from where?!
    static int minFocusPos; // TODO...
    static int maxFocusPos; // TODO...
    static float minFocusMeasure; // TODO
    static float maxFocusMeasure; // TODO
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_WIDGET_H_*/
