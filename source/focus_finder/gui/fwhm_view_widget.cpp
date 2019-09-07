#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QLabel>

#include <memory>

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>

//#include <boost/phoenix.hpp>

#include "include/fwhm_view_widget.h"

#include "../common/include/logging.h"
#include "../common/include/fwhm.h"
#include "../common/include/image.h"


FwhmViewWidgetT::FwhmViewWidgetT(QWidget * parent) : QLabel(parent)
{

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(100);
	sizePolicy.setVerticalStretch(100);

	setSizePolicy(sizePolicy);

    reset();
}

FwhmViewWidgetT::~FwhmViewWidgetT()
{
}

void FwhmViewWidgetT::drawCross(QPainter & p, const QPointF & center, const QColor & crossColor, int penWidth, int halfCrossLength) {
	p.setPen(QPen(QBrush(crossColor), penWidth, Qt::SolidLine));
	p.setBrush(QBrush(QColor(255, 255, 255, 0)));
	p.drawLine(QPointF(center.x(), center.y() - halfCrossLength), QPointF(center.x(), center.y() + halfCrossLength));
	p.drawLine(QPointF(center.x() - halfCrossLength, center.y()), QPointF(center.x() + halfCrossLength, center.y()));
}

void FwhmViewWidgetT::drawCircle(QPainter & p, const QPointF & center, const QColor & circleColor, int penWidth, int radius) {
	p.setPen(QPen(QBrush(circleColor), penWidth, Qt::SolidLine));
	p.setBrush(QBrush(QColor(255, 255, 255, 0)));
	p.drawEllipse(center, radius, radius);
}

void FwhmViewWidgetT::paintEvent(QPaintEvent * event) {
	LOG(debug) << "FwhmViewWidgetT::paintEvent..." << std::endl;

	QPainter p(this);

	if (mFwhm.valid()) {
		const auto & imgValues = mFwhm.getImgValues();
		const auto & fitValues = mFwhm.getFitValues();
		const auto & outlierValues = mFwhm.getOutlierValues();

		// Get max from imgValues and fitValues
		auto yValueExtractor = [] (const PointFT & p) { return p.y(); };

		auto yImgValues = imgValues | boost::adaptors::transformed(yValueExtractor);
		auto yFitValues = fitValues | boost::adaptors::transformed(yValueExtractor);
		auto yOutlierValues = outlierValues | boost::adaptors::transformed([](const auto & r) { return r.point.y(); });


		// HACK: Because of https://stackoverflow.com/questions/15723885/c-how-to-find-max-element-using-boostrange
		// TODO: Better way to do it??
		std::vector<float> values;
		boost::range::copy(yImgValues, std::back_inserter(values));
		boost::range::copy(yFitValues, std::back_inserter(values));
		boost::range::copy(yOutlierValues, std::back_inserter(values));

		auto maxIt = std::max_element(values.begin(), values.end());

		const float max = 1.1f * *maxIt; // Add 10% border
		const float xValueToDispScaleFactor = width() / (float) imgValues.size();
		const float yValueToDispScaleFactor = height() / max;

		auto toScreenCoords = [&](const PointFT & p) -> QPointF { return QPointF(p.x() * xValueToDispScaleFactor,
											 (height() - yValueToDispScaleFactor * p.y())); };


		LOG(debug)
			<< "FwhmT - max: " << max << ", height: " << height()
					<< ", yValueToDispScaleFactor: " << yValueToDispScaleFactor << std::endl;


		// Draw image and fit values as crosses
		const int penWidth = 1;
		const int halfCrossLength = 1;

		LOG(debug) << "FwhmViewWidgetT::paintEvent - imgValues.size(): " << imgValues.size() << ", fitValues.size(): " << fitValues.size() << std::endl;
		
		for (size_t idx = 0; idx < imgValues.size(); ++idx) {
			drawCross(p, toScreenCoords(imgValues.at(idx)), QColor(255, 0, 0, 255), penWidth, halfCrossLength);
		}

		for (size_t idx = 0; idx < fitValues.size(); ++idx) {
			drawCross(p, toScreenCoords(fitValues.at(idx)), QColor(0, 255, 0, 255), penWidth, halfCrossLength);
		}

		//Draw outliers
		for (size_t idx = 0; idx < outlierValues.size(); ++idx) {
			drawCross(p, toScreenCoords(outlierValues.at(idx).point), QColor(255, 255, 0, 255), penWidth, halfCrossLength);

			// IDEA: Draw circle radius depending on residual size
			//float residualRadius = xValueToDispScaleFactor * outlierValues.at(idx).residual;
			//drawCircle(p, toScreenCoords(outlierValues.at(idx).point), QColor(255, 255, 0, 255), penWidth, residualRadius);

			drawCircle(p, toScreenCoords(outlierValues.at(idx).point), QColor(255, 255, 0, 255), penWidth, 10.0f);
		}



		// Draw Gauss curve
		QPolygonF polygon;

		for (int i = 0; i < width(); ++i) {

		  // TODO: Need a solution for "calcGaussianValue()" - should this be part of LM package? Or duplicate code? Or use LM package directly?
			// Add point to curve polygon
			//polygon << QPointF(i, height() - yValueToDispScaleFactor * mFwhm.calcGaussianValue(i / xValueToDispScaleFactor));
		}

		p.setPen(QPen(QBrush(QColor(0, 255, 0, 255)), penWidth, Qt::SolidLine));
		p.setBrush(QBrush(QColor(255, 255, 255, 0)));
		p.drawPolyline(polygon);
	}

	Q_UNUSED(event);
}

void FwhmViewWidgetT::reset() {
	mFwhm.reset();

    repaint();
}

void FwhmViewWidgetT::setFwhm(const FwhmT & fwhm) {
	LOG(debug) << "FwhmViewWidgetT::setFwhm...mFwhm valid? " << mFwhm.valid() << std::endl;

	mFwhm = fwhm;

    repaint();
}
