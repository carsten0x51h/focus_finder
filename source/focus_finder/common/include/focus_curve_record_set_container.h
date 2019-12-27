#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORD_SET_CONTAINER_H_
#define  SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORD_SET_CONTAINER_H_

#include <memory>
#include <list>

#include "focus_curve_record_set.h"

class FocusCurveRecordSetContainerT : public std::list<std::shared_ptr<FocusCurveRecordSetT> > {
private:
  
public:
 FocusCurveRecordSetContainerT() = default;

  std::ostream & print(std::ostream & os, size_t indent = 0) const;
  friend std::ostream & operator<<(std::ostream & os, const FocusCurveRecordSetT & focusCurveRecordSet);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORD_SET_CONTAINER_H_*/

