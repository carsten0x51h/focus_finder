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
#include <QPixmap>
#include <QPen>
#include <QPoint>
#include <QResizeEvent>
#include <QLabel>

#include <memory>

#include "include/hfd_view_widget.h"


HfdViewWidgetT::HfdViewWidgetT(QWidget *parent) : QLabel(parent) {

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(100);
    sizePolicy.setVerticalStretch(100);

    setSizePolicy(sizePolicy);

    reset();
}

HfdViewWidgetT::~HfdViewWidgetT() = default;

void HfdViewWidgetT::paintEvent(QPaintEvent *event) {
    LOG(debug) << "HfdViewWidgetT::paintEvent..." << std::endl;

    QPainter p(this);

    if (mHfd.valid()) {

        const ImageT &img = mHfd.getResultImage();

        float scaleFactorX = (float) width() / (float) img.width();
        float scaleFactorY = (float) height() / (float) img.height();

        // Keep it a rect -> choose min of both ...
        float scale = std::min(scaleFactorX, scaleFactorY);

        LOG(debug) << "HfdViewWidgetT::paintEvent... scale factors=(" << scaleFactorX << "," << scaleFactorY
                   << ") -> Choose: " << scale << ", Pixmap - width: " << mStarPixmap.width()
                   << ", height: " << mStarPixmap.height() << std::endl;

        p.scale(scale, scale);
        p.drawPixmap(0, 0 /*top left of pixmap*/, mStarPixmap);


        // NOTE: +0.5 looks best... but not sure why...... probably a rounding issue.
        QPointF center(std::ceil(mStarPixmap.width() / 2) + 0.5, std::ceil(mStarPixmap.height() / 2) + 0.5);

        LOG(debug) << "HfdViewWidgetT::paintEvent...  drawing circles with center at (" << center.x() << ", "
                   << center.y() << ")..." << std::endl;

        float penWidth = 2.0F * 1.0F / (float) scale /*border width*/;
        float hfdRadius = (float) mHfd.getValue() / 2.0F;
        p.setPen(QPen(QBrush(QColor(0, 255, 0, 255)), penWidth, Qt::SolidLine));
        p.setBrush(QBrush(QColor(255, 255, 255, 0)));
        p.drawEllipse(center, hfdRadius, hfdRadius);

        float outerRadius = (float) mHfd.getOuterDiameter() / 2.0F;
        p.setPen(QPen(QBrush(QColor(255, 255, 0, 255)), penWidth, Qt::SolidLine));
        p.setBrush(QBrush(QColor(255, 255, 255, 0)));
        p.drawEllipse(center, outerRadius, outerRadius);

        // Draw a cross in the center
        const int halfCrossLength = 2;
        p.setPen(QPen(QBrush(QColor(255, 0, 0, 255)), penWidth, Qt::SolidLine));
        p.setBrush(QBrush(QColor(255, 255, 255, 0)));
        p.drawLine(QPointF(center.x(), center.y() - halfCrossLength),
                   QPointF(center.x(), center.y() + halfCrossLength));
        p.drawLine(QPointF(center.x() - halfCrossLength, center.y()),
                   QPointF(center.x() + halfCrossLength, center.y()));
    }

    Q_UNUSED(event)
}

void HfdViewWidgetT::reset() {
    mHfd.reset();

    repaint();
}


// NOTE: Return by value - should not be a problem since image is quite small
QPixmap HfdViewWidgetT::convertToPixmap(const ImageT &img) {
    float min;
    float max = img.max_min(min);
    const float mm = max - min;

    LOG(debug) << "HfdViewPanelT::convertToPixmap... min=" << min << ", max=" << max << ", mm=" << mm << std::endl;

    // Finally, put the result into an QImage.
    QImage qtImage(img.width(), img.height(),
                   QImage::Format_ARGB32);

    cimg_forXY(img, x, y) {
            //int pixValue = displayImg(x, y, 0);
            int pixValue = (int) (255.0F * (float) (img(x, y, 0) - min) / mm);
            qtImage.setPixel(x, y, qRgb(pixValue, pixValue, pixValue));
        }

    return QPixmap::fromImage(qtImage);
}


void HfdViewWidgetT::setHfd(const HfdT &hfd) {
    mHfd = hfd;

    LOG(debug) << "HfdViewWidgetT::setHfd...mHfd valid? " << mHfd.valid() << std::endl;

    mStarPixmap = (mHfd.valid() ? convertToPixmap(mHfd.getResultImage()) : QPixmap());

    repaint();
}
