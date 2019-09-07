#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QPixmap>
#include <QPoint>
#include <QResizeEvent>
#include <QLabel>

#include <memory>

#include "include/focus_curve_view_widget.h"

#include "../common/include/logging.h"
#include "../common/include/image.h"


FocusCurveViewWidgetT::FocusCurveViewWidgetT(QWidget * parent) : QLabel(parent)
{

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(100);
	sizePolicy.setVerticalStretch(100);

	setSizePolicy(sizePolicy);

    reset();
}

FocusCurveViewWidgetT::~FocusCurveViewWidgetT()
{
}

void FocusCurveViewWidgetT::paintEvent(QPaintEvent * event) {
	LOG(debug) << "FocusCurveViewWidgetT::paintEvent..." << std::endl;

	QPainter p(this);

	// HACK... TODO: Just for test
	QPointF center(10.0, 10.0);
	p.setPen(QPen(QBrush(QColor(0, 255, 0, 255)), 1, Qt::SolidLine));
	p.setBrush(QBrush(QColor(255, 255, 255, 0)));
	p.drawEllipse(center, 5 /*radius*/, 5 /*radius*/);

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

	Q_UNUSED(event);
}

void FocusCurveViewWidgetT::reset() {
	//TODO: mFocusCurve.reset();

    repaint();
}

void FocusCurveViewWidgetT::setFocusCurve(const FocusCurveT & focusCurve) {
	mFocusCurve = focusCurve;

	LOG(debug) << "FocusCurveViewWidgetT::setFocusCurve..." << std::endl;

	// TODO

    repaint();
}


