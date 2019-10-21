#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORD_SET_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORD_SET_H_

#include <memory>
#include <fstream>
#include <vector>
#include <tuple>
#include <list>

#include "focus_curve_record.h"
#include "focus_measure_type.h"

#include "image.h"
#include "point.h"

class FocusCurveRecordSetT : public std::vector< std::shared_ptr<FocusCurveRecordT> > {
private:
  FocusMeasureTypeT::TypeE mFocusMeasureType;
  float mFocusMeasureLimit;
  
public:
  FocusCurveRecordSetT(FocusMeasureTypeT::TypeE focusMeasureType, float focusMeasureLimit);
  FocusMeasureTypeT::TypeE getFocusMeasureType() const;
  float getFocusMeasureLimit() const;
  std::pair<int, int> minmaxFocusPos() const;
  
  std::ostream & print(std::ostream & os) const;
  friend std::ostream & operator<<(std::ostream & os, const FocusCurveRecordSetT & focusCurveRecordSet);
};

typedef std::list<std::shared_ptr<FocusCurveRecordSetT> > FocusCurveRecordSetContainerT;

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORD_SET_H_*/




