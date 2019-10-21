#include <tuple>

#include "include/focus_curve_record_set.h"
#include "include/focus_measure_type.h"

FocusCurveRecordSetT::FocusCurveRecordSetT(FocusMeasureTypeT::TypeE focusMeasureType, float focusMeasureLimit) : mFocusMeasureType(focusMeasureType), mFocusMeasureLimit(focusMeasureLimit) {
}

float FocusCurveRecordSetT::getFocusMeasureLimit() const {
  return mFocusMeasureLimit;
}

FocusMeasureTypeT::TypeE FocusCurveRecordSetT::getFocusMeasureType() const {
  return mFocusMeasureType;
}

/**
 * Find min and max position in all records of this set.
 */
std::pair<int, int> FocusCurveRecordSetT::minmaxFocusPos() const {

  const_iterator itMin;
  const_iterator itMax;
  
  std::tie(itMin, itMax) = std::minmax_element(std::begin(*this),
					       std::end(*this),
					       [] (std::shared_ptr<const FocusCurveRecordT> focusCurveRecord1, std::shared_ptr<const FocusCurveRecordT> focusCurveRecord2)
					       {
						 return focusCurveRecord1->getCurrentAbsoluteFocusPos() < focusCurveRecord2->getCurrentAbsoluteFocusPos();
					       });
  
  return std::make_pair<int, int>((**itMin).getCurrentAbsoluteFocusPos(), (**itMax).getCurrentAbsoluteFocusPos());
}


std::ostream & operator<<(std::ostream & os, const FocusCurveRecordSetT & focusCurveRecordSet) {
	return focusCurveRecordSet.print(os);
}

std::ostream &
FocusCurveRecordSetT::print(std::ostream & os) const {

  os << "--- FocusCurveRecordSet (#" << this->size() << " data points) ---" << std::endl;
  
  for (FocusCurveRecordSetT::const_iterator it = this->begin(); it != this->end(); ++it) {
    const FocusCurveRecordT & fcr = **it;
    
    os << "POS=" << fcr.getCurrentAbsoluteFocusPos()
       << ", SNR=" << fcr.getSnr()
       << ", HFD=" << fcr.getHfd().getValue()
       << ", FWHM(H)=" << fcr.getFwhmHorz().getValue()
       << ", FWHM(V)=" << fcr.getFwhmVert().getValue()
       << ", STAR DRIFT(dx,dy)=(" << std::get<0>(fcr.getDrift()) << ", " << std::get<1>(fcr.getDrift()) << ")"
       << std::endl;
  }
  return os;
}
