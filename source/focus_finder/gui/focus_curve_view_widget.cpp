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

#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QPixmap>
#include <QPoint>
#include <QResizeEvent>
#include <QLabel>
#include <QColor>

#include <memory>

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>

#include "include/focus_curve_view_widget.h"

#include "../common/include/logging.h"
#include "../common/include/image.h"
#include "../common/include/focus_curve_record_set.h"
#include "../common/include/focus_curve_recorder_logic.h"
#include "../common/include/focus_measure_type.h"
#include "../common/include/focus_curve.h"
#include "../common/include/point.h"


// HACK: TODO: Do not hardcode!! Move away...
int FocusCurveViewWidgetT::minFocusPos = 0;
int FocusCurveViewWidgetT::maxFocusPos = 80000;
float FocusCurveViewWidgetT::minFocusMeasure = 0.0F;
float FocusCurveViewWidgetT::maxFocusMeasure = 20.0F;


FocusCurveViewWidgetT::FocusCurveViewWidgetT(QWidget *parent,
                                             std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic)
        : QLabel(parent), mFocusCurveRecorderLogic(focusCurveRecorderLogic), mFocusCurveHack(nullptr) {
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(100);
    sizePolicy.setVerticalStretch(100);

    setSizePolicy(sizePolicy);

    setMouseTracking(true);

    reset();
}

QPointF FocusCurveViewWidgetT::transformToScreenCoordinates(float focusPos, float focusMeasure) {
    int deltaFocusPos = maxFocusPos - minFocusPos;
    float deltaFocusMeasure = maxFocusMeasure - minFocusMeasure;

    // Transform coordinates
    float xpos = (focusPos / (float) deltaFocusPos) * width();
    float ypos = (1.0F - (focusMeasure / deltaFocusMeasure)) * height();

    return QPointF(xpos, ypos);
}

FocusCurveViewWidgetT::~FocusCurveViewWidgetT() {
}

std::vector<PointFT> dataPoints;

void FocusCurveViewWidgetT::drawFocusCurveRecordSet(QPainter *p, const std::vector<PointFT> &dataPoints,
                                                    const FocusFinderProfileT &focusFinderProfile, QColor color) {

    // Min / max focus positions
    int deltaFocusPos = maxFocusPos - minFocusPos;
    float deltaFocusMeasure = maxFocusMeasure - minFocusMeasure;

    for (std::vector<PointFT>::const_iterator it = dataPoints.begin(); it != dataPoints.end(); ++it) {

        float focusPos = it->x();
        float focusMeasure = it->y();

        // Transform coordinates
        QPointF center = transformToScreenCoordinates(focusPos, focusMeasure);

        // NOTE: This one produces a lot of output...
        // LOG(debug) << "FocusCurveViewWidgetT::drawFocusCurveRecordSet... focusPos=" << focusPos << ", focusMeasure=" << focusMeasure << ", width=" << width() << ", draw point (x, y)=" << center.x() << ", " << center.y() << ")." << std::endl;

        p->setPen(QPen(QBrush(color), 1, Qt::SolidLine));
        p->setBrush(QBrush(QColor(255, 255, 255, 0)));
        p->drawEllipse(center, 5 /*radius*/, 5 /*radius*/);
    }
}

void FocusCurveViewWidgetT::drawFocusCurveRecordSets(QPainter *p) {
    auto focusCurveRecorder = mFocusCurveRecorderLogic->getFocusCurveRecorder();

    if (focusCurveRecorder == nullptr) {
        return;
    }

    auto focusAnalyzer = focusCurveRecorder->getFocusController();

    if (focusAnalyzer == nullptr) {
        return;
    }

    const FocusFinderProfileT &focusFinderProfile = focusAnalyzer->getFocusFinderProfile();

    // TODO: Should this widget really use the FocusCurveRecorder? Or should the RecordSets being set from outside?

    if (mFocusCurveHack) {
        const CurveFitSummaryT &curveFitSummary = mFocusCurveHack->getCurveFitSummary();

        //LOG(debug) << "FocusCurveViewWidgetT::drawFocusCurveRecordSets... drawing curveFitSummary.matchedDataPoints..." << std::endl;

        drawFocusCurveRecordSet(p, curveFitSummary.matchedDataPoints, focusFinderProfile, QColor(0, 255, 0, 255));



        //LOG(debug) << "FocusCurveViewWidgetT::drawFocusCurveRecordSets... drawing curveFitSummary.outliers..." << std::endl;

        std::vector<PointFT> outlierPoints;

        auto extractOutlierPointsFunction = [&](const PointWithResidualT &outlier) { return outlier.point; };
        boost::range::copy(curveFitSummary.outliers | boost::adaptors::transformed(extractOutlierPointsFunction),
                           std::back_inserter(outlierPoints));

        drawFocusCurveRecordSet(p, outlierPoints, focusFinderProfile, QColor(255, 255, 0, 255));


    } else if (focusCurveRecorder != nullptr) {
        auto focusCurveRecordSets = focusCurveRecorder->getFocusCurveRecordSets();
        // FocusMeasureTypeT::TypeE focusMeasureType = focusCurveRecorder->getFocusMeasureType();

        LOG(debug) << "FocusCurveViewWidgetT::drawFocusCurveRecordSets...focusCurveRecordSets.size(): "
                   << focusCurveRecordSets->size() << std::endl;

        for (const auto &recordSet : *focusCurveRecordSets) {

            std::vector<PointFT> dataPoints;

            auto transformRecordSetToPointFunction = [&](std::shared_ptr<FocusCurveRecordT> rs) {
                return PointFT(
                        rs->getCurrentAbsoluteFocusPos(),
                        rs->getFocusMeasure(focusFinderProfile.getCurveFocusMeasureType())
                );
            };

            boost::range::copy(*recordSet | boost::adaptors::transformed(transformRecordSetToPointFunction),
                               std::back_inserter(dataPoints));

            // TODO: Draw each in a different color...?!
            drawFocusCurveRecordSet(p, dataPoints, focusFinderProfile, QColor(0, 255, 0, 255));
        }
    } else {
        LOG(debug) << "FocusCurveViewWidgetT::drawFocusCurveRecordSets... nothing to draw... no focus curve recorder."
                   << std::endl;
    }
}

// FocusMeasureTypeT::TypeE FocusCurveViewWidgetT::getFocusMeasureType() const {
//   return mFocusMeasureType;
// }

// void FocusCurveViewWidgetT::setFocusMeasureType(FocusMeasureTypeT::TypeE focusMeasureType) {
//   mFocusMeasureType = focusMeasureType;
// }


void FocusCurveViewWidgetT::update() {
    repaint();
}


void FocusCurveViewWidgetT::drawCurveHack(std::shared_ptr<FocusCurveT> focusCurve) {
    mFocusCurveHack = focusCurve;
    repaint();
}

void FocusCurveViewWidgetT::paintEvent(QPaintEvent *event) {

    QPainter p(this);

    // TODO: Make the view widget to allow multiple "curves"? HFD, FWHN etc...
    drawFocusCurveRecordSets(&p);


    // Draw focus curve
    p.setPen(QPen(QBrush(QColor(255, 0, 0, 255)), 1, Qt::SolidLine));
    p.setBrush(QBrush(QColor(255, 255, 255, 0)));

    if (mFocusCurveHack != nullptr) {
        // TODO: Later use the boundaries of the curve instead of min/max focus pos!
        int lowerFocusPos = mFocusCurveHack->getLowerFocusPos();
        int upperFocusPos = mFocusCurveHack->getUpperFocusPos();

        for (int xScreen = 0; xScreen < width(); ++xScreen) {

            float focusPos =
                    lowerFocusPos + ((float) xScreen / (float) width()) * (float) (upperFocusPos - lowerFocusPos);
            float focusMeasure = mFocusCurveHack->calcFocusMeasureByFocusPosition(focusPos);

            QPointF center = transformToScreenCoordinates(focusPos, focusMeasure);
            //     focusMeasureScaled
            // p.drawLine(QPointF(mLastMousePos.x(), 0), QPointF(mLastMousePos.x(), height()));

            p.drawEllipse(center, 2 /*radius*/, 2 /*radius*/);

            // TODO: Paint...
        }
    }


//	if (mHfd.valid()) {
//
//		const ImageT & img = mHfd.getResultImage();
//
//		float scaleFactorX = (float) width() / (float) img.width();
//		float scaleFactorY = (float) height() / (float) img.height();
//
//		// Keep it a rect -> choose min of both ...
//		float scale = std::min(scaleFactorX, scaleFactorY);
//
//		LOG(debug) << "FocusCurveViewWidgetT::paintEvent... scale factors=(" << scaleFactorX << "," << scaleFactorY
//				   << ") -> Choose: " << scale <<  ", Pixmap - width: " << mStarPixmap.width()
//				   << ", height: " << mStarPixmap.height() << std::endl;
//
//		p.scale(scale, scale);
//		p.drawPixmap(0, 0 /*top left of pixmap*/, mStarPixmap);
//
//		// NOTE: +0.5 looks best... but not sure why...... probably a rounding issue.
//		QPointF center(std::ceil(mStarPixmap.width() / 2) + 0.5, std::ceil(mStarPixmap.height() / 2) + 0.5);
//
//		LOG(debug) << "FocusCurveViewWidgetT::paintEvent...  drawing circles with center at (" << center.x() << ", " << center.y() << ")..." << std::endl;
//
//		float penWidth = 2.0 * 1.0 / (float) scale /*border width*/;
//		float hfdRadius = mHfd.getValue() / 2.0;
//		p.setPen(QPen(QBrush(QColor(0, 255, 0, 255)), penWidth, Qt::SolidLine));
//		p.setBrush(QBrush(QColor(255, 255, 255, 0)));
//		p.drawEllipse(center, hfdRadius, hfdRadius);
//
//		float outerRadius = mHfd.getOuterDiameter() / 2.0;
//		p.setPen(QPen(QBrush(QColor(255, 255, 0, 255)), penWidth, Qt::SolidLine));
//		p.setBrush(QBrush(QColor(255, 255, 255, 0)));
//		p.drawEllipse(center, outerRadius, outerRadius);
//
//		// Draw a cross in the center
//		const int halfCrossLength = 2;
//		p.setPen(QPen(QBrush(QColor(255, 0, 0, 255)), penWidth, Qt::SolidLine));
//		p.setBrush(QBrush(QColor(255, 255, 255, 0)));
//		p.drawLine(QPointF(center.x(), center.y() - halfCrossLength), QPointF(center.x(), center.y() + halfCrossLength));
//		p.drawLine(QPointF(center.x() - halfCrossLength, center.y()), QPointF(center.x() + halfCrossLength, center.y()));
//	}

    // Draw cross
    p.setPen(QPen(QBrush(QColor(255, 255, 0, 255)), 2, Qt::SolidLine));
    p.setBrush(QBrush(QColor(255, 255, 255, 0)));
    p.drawLine(QPointF(mLastMousePos.x(), 0), QPointF(mLastMousePos.x(), height()));
    p.drawLine(QPointF(0, mLastMousePos.y()), QPointF(width(), mLastMousePos.y()));

    Q_UNUSED(event);
}

void FocusCurveViewWidgetT::reset() {
    //mFocusCurve.clear();

    repaint();
}

// void FocusCurveViewWidgetT::setFocusCurve(std::shared_ptr<const FocusCurveT> focusCurve) {
// 	mFocusCurve = focusCurve;

// 	LOG(debug) << "FocusCurveViewWidgetT::setFocusCurve..." << std::endl;

// 	// TODO

//     repaint();
// }

void FocusCurveViewWidgetT::mouseMoveEvent(QMouseEvent *event) {
    mLastMousePos = event->pos(); // NOTE: Requires mouse tracking
    repaint();
}
