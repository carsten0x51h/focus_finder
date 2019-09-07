#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QPixmap>
#include <QPoint>
#include <QResizeEvent>
#include <QLabel>

#include <memory>

#include "include/hfd_view_widget.h"

#include "../common/include/logging.h"
#include "../common/include/hfd.h"
#include "../common/include/image.h"


HfdViewWidgetT::HfdViewWidgetT(QWidget * parent) : QLabel(parent)
{

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(100);
	sizePolicy.setVerticalStretch(100);

	setSizePolicy(sizePolicy);

    reset();
}

HfdViewWidgetT::~HfdViewWidgetT()
{
}

void HfdViewWidgetT::paintEvent(QPaintEvent * event) {
	LOG(debug) << "HfdViewWidgetT::paintEvent..." << std::endl;

	QPainter p(this);

	if (mHfd.valid()) {

		const ImageT & img = mHfd.getResultImage();

		float scaleFactorX = (float) width() / (float) img.width();
		float scaleFactorY = (float) height() / (float) img.height();

		// Keep it a rect -> choose min of both ...
		float scale = std::min(scaleFactorX, scaleFactorY);

		LOG(debug) << "HfdViewWidgetT::paintEvent... scale factors=(" << scaleFactorX << "," << scaleFactorY
				   << ") -> Choose: " << scale <<  ", Pixmap - width: " << mStarPixmap.width()
				   << ", height: " << mStarPixmap.height() << std::endl;

		p.scale(scale, scale);
		p.drawPixmap(0, 0 /*top left of pixmap*/, mStarPixmap);


		// NOTE: +0.5 looks best... but not sure why...... probably a rounding issue.
		QPointF center(std::ceil(mStarPixmap.width() / 2) + 0.5, std::ceil(mStarPixmap.height() / 2) + 0.5);

		LOG(debug) << "HfdViewWidgetT::paintEvent...  drawing circles with center at (" << center.x() << ", " << center.y() << ")..." << std::endl;

		float penWidth = 2.0 * 1.0 / (float) scale /*border width*/;
		float hfdRadius = mHfd.getValue() / 2.0;
		p.setPen(QPen(QBrush(QColor(0, 255, 0, 255)), penWidth, Qt::SolidLine));
		p.setBrush(QBrush(QColor(255, 255, 255, 0)));
		p.drawEllipse(center, hfdRadius, hfdRadius);

		float outerRadius = mHfd.getOuterDiameter() / 2.0;
		p.setPen(QPen(QBrush(QColor(255, 255, 0, 255)), penWidth, Qt::SolidLine));
		p.setBrush(QBrush(QColor(255, 255, 255, 0)));
		p.drawEllipse(center, outerRadius, outerRadius);

		// Draw a cross in the center
		const int halfCrossLength = 2;
		p.setPen(QPen(QBrush(QColor(255, 0, 0, 255)), penWidth, Qt::SolidLine));
		p.setBrush(QBrush(QColor(255, 255, 255, 0)));
		p.drawLine(QPointF(center.x(), center.y() - halfCrossLength), QPointF(center.x(), center.y() + halfCrossLength));
		p.drawLine(QPointF(center.x() - halfCrossLength, center.y()), QPointF(center.x() + halfCrossLength, center.y()));
	}

	Q_UNUSED(event);
}

void HfdViewWidgetT::reset() {
	mHfd.reset();

    repaint();
}


// NOTE: Return by value - should not be a problem since image is quite small
QPixmap HfdViewWidgetT::convertToPixmap(const ImageT & img) {
	float min;
	float max = img.max_min(min);
	const float mm = max - min;

	LOG(debug) << "HfdViewPanelT::convertToPixmap... min=" << min << ", max=" << max << ", mm=" << mm << std::endl;

	// Finally, put the result into an QImage.
	QImage qtImage(img.width(), img.height(),
			QImage::Format_ARGB32);

	cimg_forXY(img, x, y)
	{
		//int pixValue = displayImg(x, y, 0);
		int pixValue = 255.0 * (img(x, y, 0) - min) / mm;
		qtImage.setPixel(x, y, qRgb(pixValue, pixValue, pixValue));
	}

	return QPixmap::fromImage(qtImage);
}


void HfdViewWidgetT::setHfd(const HfdT & hfd) {
	mHfd = hfd;

	LOG(debug) << "HfdViewWidgetT::setHfd...mHfd valid? " << mHfd.valid() << std::endl;

	mStarPixmap = (mHfd.valid() ? convertToPixmap(mHfd.getResultImage()) : QPixmap());

    repaint();
}
