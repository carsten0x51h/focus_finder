#include "include/self_orientation_result.h"
#include "include/focus_curve_record.h"


std::ostream &
SelfOrientationResultT::print(std::ostream & os) const {
  os << "--- SelfOrientationResultT ---" << std::endl
     << "Focus direction to closest limit: " << FocusDirectionT::asStr(focusDirectionToLimit) << std::endl
     << "We are in curve half: " << CurveHalfT::asStr(curveHalf) << std::endl
     << "Record1: " << std::endl
     << *record1 << std::endl
     << "Record2: " << std::endl
     << *record2;
}

std::ostream & operator<<(std::ostream & os, const SelfOrientationResultT & selfOrientationResult) {
  return selfOrientationResult.print(os);
}

