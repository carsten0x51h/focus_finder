#include <algorithm>
//#include <gsl/gsl_vector.h>

#include "include/curve_parms.h"
#include "include/curve_parm.h"

// CurveParmsT::CurveParmsT(const gsl_vector * inVec = 0, const std::vector<std::string> & inNames = {}) {
// 	for (size_t idx = 0; idx < inVec->size; ++idx) {
// 		float value = (inVec ? gsl_vector_get(inVec, idx) : 0.0f);
// 		std::string name = (inNames.size() > idx ? inNames.at(idx) : "<unknown>");

// 		mCurveParms.push_back(CurveParmT(name, value));
// 	}
// }
CurveParmsT::CurveParmsT() {
}

CurveParmsT::CurveParmsT(size_t size) {
  mCurveParms.resize(size);
}

const CurveParmT & CurveParmsT::get(size_t idx) const {

	if (idx >= mCurveParms.size()) {
		std::stringstream ss;
		ss << "Index '" << idx << "' out of bounds (size=" << mCurveParms.size()
				<< ")." << std::endl;
		throw CurveParmExceptionT(ss.str());
	}

	return mCurveParms.at(idx);
}

const CurveParmT & CurveParmsT::get(const std::string & name) const {

	auto iter = std::find_if(mCurveParms.begin(), mCurveParms.end(),
			[& name](const CurveParmT & obj) {return obj.getName() == name;});

	if (iter == mCurveParms.end()) {
		std::stringstream ss;
		ss << "Curve parameter with name '" << name << "' not found."
				<< std::endl;
		throw CurveParmExceptionT(ss.str());
	}

	return *iter;
}

size_t CurveParmsT::size() const {
  return mCurveParms.size();
}

CurveParmT & CurveParmsT::operator[](size_t idx) {
	if (idx >= mCurveParms.size()) {
		std::stringstream ss;
		ss << "Index '" << idx << "' out of bounds (size=" << mCurveParms.size()
				<< ")." << std::endl;
		throw CurveParmExceptionT(ss.str());
	}

	return mCurveParms[idx];
}

std::ostream &
CurveParmsT::print(std::ostream & os) const {
	for (auto & cp : mCurveParms) {
		os << cp << std::endl;
	}
	return os;
}

std::ostream & operator<<(std::ostream & os, const CurveParmsT & curveParms) {
	return curveParms.print(os);
}

