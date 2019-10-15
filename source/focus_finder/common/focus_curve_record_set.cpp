#include "include/focus_curve_record_set.h"
#include "include/focus_measure_type.h"

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
