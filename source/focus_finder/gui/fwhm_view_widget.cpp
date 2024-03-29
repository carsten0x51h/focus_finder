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

#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QLabel>

#include <memory>

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>

#include "include/fwhm_view_widget.h"


FwhmViewWidgetT::FwhmViewWidgetT(QWidget *parent) : QLabel(parent) {

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(100);
    sizePolicy.setVerticalStretch(100);

    setSizePolicy(sizePolicy);

    reset();
}

FwhmViewWidgetT::~FwhmViewWidgetT() = default;

// TODO: Maybe use "DrawHelperT" ?
void FwhmViewWidgetT::drawCross(QPainter &p, const QPointF &center, const QColor &crossColor, int penWidth,
                                int halfCrossLength) {
    p.setPen(QPen(QBrush(crossColor), penWidth, Qt::SolidLine));
    p.setBrush(QBrush(Qt::transparent));
    p.drawLine(QPointF(center.x(), center.y() - halfCrossLength), QPointF(center.x(), center.y() + halfCrossLength));
    p.drawLine(QPointF(center.x() - halfCrossLength, center.y()), QPointF(center.x() + halfCrossLength, center.y()));
}

void
FwhmViewWidgetT::drawCircle(QPainter &p, const QPointF &center, const QColor &circleColor, int penWidth, int radius) {
    p.setPen(QPen(QBrush(circleColor), penWidth, Qt::SolidLine));
    p.setBrush(QBrush(Qt::transparent));
    p.drawEllipse(center, radius, radius);
}

void FwhmViewWidgetT::paintEvent(QPaintEvent *event) {
    LOG(debug) << "FwhmViewWidgetT::paintEvent..." << std::endl;

    QPainter p(this);

    if (mFwhm.valid()) {
        const auto &imgValues = mFwhm.getImgValues();
        const auto &fitValues = mFwhm.getFitValues();
        const auto &outlierValues = mFwhm.getOutlierValues();

        // Get max from imgValues and fitValues
        auto yValueExtractor = [](const PointFT &p) { return p.y(); };

        auto yImgValues = imgValues | boost::adaptors::transformed(yValueExtractor);
        auto yFitValues = fitValues | boost::adaptors::transformed(yValueExtractor);
        auto yOutlierValues = outlierValues | boost::adaptors::transformed([](const auto &r) { return r.point.y(); });


        // HACK: Because of https://stackoverflow.com/questions/15723885/c-how-to-find-max-element-using-boostrange
        // TODO: Better way to do it??
        std::vector<float> values;
        boost::range::copy(yImgValues, std::back_inserter(values));
        boost::range::copy(yFitValues, std::back_inserter(values));
        boost::range::copy(yOutlierValues, std::back_inserter(values));

        auto maxIt = std::max_element(values.begin(), values.end());

        const float max = 1.1F * *maxIt; // Add 10% border
        const float xValueToDispScaleFactor = (float) width() / (float) imgValues.size();
        const float yValueToDispScaleFactor = (float) height() / max;

        auto toScreenCoords = [&](const PointFT &p) -> QPointF {
            return QPointF(p.x() * xValueToDispScaleFactor,
                           ((float) height() - yValueToDispScaleFactor * p.y()));
        };


        LOG(debug)
            << "FwhmT - max: " << max << ", height: " << height()
            << ", yValueToDispScaleFactor: " << yValueToDispScaleFactor << std::endl;


        // Draw image and fit values as crosses
        const int penWidth = 1;
        const int halfCrossLength = 1;

        LOG(debug) << "FwhmViewWidgetT::paintEvent - imgValues.size(): " << imgValues.size() << ", fitValues.size(): "
                   << fitValues.size() << std::endl;

        for (const auto & imgValue : imgValues) {
            drawCross(p, toScreenCoords(imgValue), Qt::red, penWidth, halfCrossLength);
        }

        for (const auto & fitValue : fitValues) {
            drawCross(p, toScreenCoords(fitValue), Qt::red, penWidth, halfCrossLength);
        }

        //Draw outliers
        for (const auto & outlierValue : outlierValues) {
            drawCross(p, toScreenCoords(outlierValue.point), Qt::yellow, penWidth,
                      halfCrossLength);

            // IDEA: Draw circle radius depending on residual size
            //float residualRadius = xValueToDispScaleFactor * outlierValues.at(idx).residual;
            //drawCircle(p, toScreenCoords(outlierValues.at(idx).point), QColor(255, 255, 0, 255), penWidth, residualRadius);

            drawCircle(p, toScreenCoords(outlierValue.point), Qt::yellow, penWidth, 10.0F);
        }



        // Draw Gauss curve
        QPolygonF polygon;

        for (int i = 0; i < width(); ++i) {

            // TODO: Need a solution for "calcGaussianValue()" - should this be part of LM package? Or duplicate code? Or use LM package directly?
            // Add point to curve polygon
            //polygon << QPointF(i, height() - yValueToDispScaleFactor * mFwhm.calcGaussianValue(i / xValueToDispScaleFactor));
        }

        p.setPen(QPen(QBrush(Qt::green), penWidth, Qt::SolidLine));
        p.setBrush(QBrush(Qt::transparent));
        p.drawPolyline(polygon);
    }

    Q_UNUSED(event)
}

void FwhmViewWidgetT::reset() {
    mFwhm.reset();

    repaint();
}

void FwhmViewWidgetT::setFwhm(const FwhmT &fwhm) {
    LOG(debug) << "FwhmViewWidgetT::setFwhm...mFwhm valid? " << mFwhm.valid() << std::endl;

    mFwhm = fwhm;

    repaint();
}
