#include "include/focus_curve.h"


// TODO: Inherrit from std::vector????! Or add() ?

FocusCurveT::FocusCurveT() {

}

FocusCurveT::~FocusCurveT() {

}

std::ostream & operator<<(std::ostream & os, const FocusCurveT & focusCurve) {
	return focusCurve.print(os);
}

std::ostream &
FocusCurveT::print(std::ostream & os) const {
	os << "FocusCurveT...TODO" << std::endl;
	return os;
}



