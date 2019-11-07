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

#include "../../common/include/image.h"
#include "../../common/include/focus_measure_type.h"

class FocusCurveRecordT;
class FocusCurveRecordSetT;
class FocusCurveRecorderT;
class FocusCurveRecorderLogicT;

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

  // FocusMeasureTypeT::TypeE getFocusMeasureType() const;
  // void setFocusMeasureType(FocusMeasureTypeT::TypeE focusMeasureType);
  
  void paintEvent(QPaintEvent * event) override;
  void mouseMoveEvent(QMouseEvent * event);


  
signals:

protected slots:

protected:

private:
  void drawFocusCurveRecordSet(QPainter * p, std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet, std::shared_ptr<FocusCurveRecorderT> focusCurveRecorder);
  void drawFocusCurveRecordSets(QPainter * p);

  std::shared_ptr<FocusCurveRecorderLogicT> mFocusCurveRecorderLogic;
  
  QPoint mLastMousePos;
  //  FocusMeasureTypeT::TypeE mFocusMeasureType;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_VIEW_WIDGET_H_*/
