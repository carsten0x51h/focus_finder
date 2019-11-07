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
#include "../common/include/focus_curve_record_set.h"
#include "../common/include/focus_curve_recorder_logic.h"
#include "../common/include/focus_measure_type.h"

FocusCurveViewWidgetT::FocusCurveViewWidgetT(QWidget * parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic) : QLabel(parent), mFocusCurveRecorderLogic(focusCurveRecorderLogic)
{
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(100);
  sizePolicy.setVerticalStretch(100);
  
  setSizePolicy(sizePolicy);
  
  setMouseTracking(true);
  
  reset();
}

FocusCurveViewWidgetT::~FocusCurveViewWidgetT()
{
}
void FocusCurveViewWidgetT::drawFocusCurveRecordSet(QPainter * p, std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet, std::shared_ptr<FocusCurveRecorderT> focusCurveRecorder) {

  // Min / max focus positions
  int minFocusPos = 0; // TODO...
  int maxFocusPos = 80000; // TODO...
  int deltaFocusPos = maxFocusPos - minFocusPos;

  float minFocusMeasure = 0; // TODO
  float maxFocusMeasure = 20; // TODO
  float deltaFocusMeasure = maxFocusMeasure - minFocusMeasure;

  const FocusFinderProfileT & focusFinderProfile = focusCurveRecorder->getFocusFinderProfile();
  
  for (FocusCurveRecordSetT::const_iterator it = focusCurveRecordSet->begin(); it != focusCurveRecordSet->end(); ++it) {

	  auto fcr = *it;

	  float focusPos = fcr->getCurrentAbsoluteFocusPos();
	  float focusMeasure = fcr->getFocusMeasure(focusFinderProfile.getCurveFocusMeasureType());
	    
	  // Transform coordinates
	  float xpos = (focusPos / (float) deltaFocusPos) * width();
	  float ypos = (1.0F - (focusMeasure / deltaFocusMeasure)) * height();

	  LOG(debug) << "FocusCurveViewWidgetT::drawFocusCurveRecordSet... focusPos=" << focusPos << ", width=" << width() << ", draw point (x, y)=" << xpos << ", " << ypos << ")." << std::endl;
	  
	  QPointF center(xpos, ypos);
	  p->setPen(QPen(QBrush(QColor(0, 255, 0, 255)), 1, Qt::SolidLine));
	  p->setBrush(QBrush(QColor(255, 255, 255, 0)));
	  p->drawEllipse(center, 5 /*radius*/, 5 /*radius*/);
	}
  
}

void FocusCurveViewWidgetT::drawFocusCurveRecordSets(QPainter * p) {
  auto focusCurveRecorder = mFocusCurveRecorderLogic->getFocusCurveRecorder();

  // TODO: Should this widget really use the FocusCurveRecorder? Or should the RecordSets being set from outside?
  
  if (focusCurveRecorder != nullptr) {
    auto focusCurveRecordSets = focusCurveRecorder->getFocusCurveRecordSets();
    // FocusMeasureTypeT::TypeE focusMeasureType = focusCurveRecorder->getFocusMeasureType();

    
    LOG(debug) << "FocusCurveViewWidgetT::drawFocusCurveRecordSets...focusCurveRecordSets.size(): " << focusCurveRecordSets->size() << std::endl;

    
    for (const auto & recordSet : *focusCurveRecordSets) {
      // TODO: Draw each in a different color...?!
      drawFocusCurveRecordSet(p, recordSet, focusCurveRecorder);
    }	  
  } else {
    LOG(debug) << "FocusCurveViewWidgetT::drawFocusCurveRecordSets... nothing to draw... no focus curve recorder." << std::endl;
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

void FocusCurveViewWidgetT::paintEvent(QPaintEvent * event) {

	QPainter p(this);

	// TODO: Make the view widget to allow multiple "curves"? HFD, FWHN etc...	

	drawFocusCurveRecordSets(& p);
	

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

void FocusCurveViewWidgetT::mouseMoveEvent(QMouseEvent * event) {
  mLastMousePos = event->pos(); // NOTE: Requires mouse tracking
  repaint();
}
