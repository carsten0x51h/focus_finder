#include "include/focus_curve_record_set_container.h"
  
std::ostream & FocusCurveRecordSetContainerT::print(std::ostream & os, size_t indent) const {
  std::string prefix = std::string(indent, ' ');
  
  os << prefix << "--- FocusCurveRecordSetContainer (#" << this->size() << " record sets) ---" << std::endl;
  
  for (FocusCurveRecordSetContainerT::const_iterator it = this->begin(); it != this->end(); ++it) {
    const FocusCurveRecordSetT & fcrs = **it;
    fcrs.print(os, indent + 4);
  }

  return os;
}

std::ostream & operator<<(std::ostream & os, const FocusCurveRecordSetContainerT & focusCurveRecordSetContainer) {
  return focusCurveRecordSetContainer.print(os);
}
