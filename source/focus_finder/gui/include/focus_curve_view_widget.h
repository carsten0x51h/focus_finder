#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_WIDGET_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_WIDGET_H_

// std includes
#include <memory>

// Qt includes
//#include <QWidget>
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
class FocusCurveViewWidgetT : public QLabel
{
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
	FocusCurveViewWidgetT(QWidget * parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FocusCurveViewWidgetT();

  void reset();
  //void setFocusCurve(std::shared_ptr<const FocusCurveT> focusCurve);
  void update();
  void drawCurveHack(std::shared_ptr<FocusCurveT> focusCurve);

  // FocusMeasureTypeT::TypeE getFocusMeasureType() const;
  // void setFocusMeasureType(FocusMeasureTypeT::TypeE focusMeasureType);
  
  void paintEvent(QPaintEvent * event) override;
  void mouseMoveEvent(QMouseEvent * event);


  
signals:

protected slots:

protected:

private:
  void drawFocusCurveRecordSet(QPainter * p, const std::vector<PointFT> & dataPoints, const FocusFinderProfileT & focusFinderProfile, QColor color);
  void drawFocusCurveRecordSets(QPainter * p);

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
