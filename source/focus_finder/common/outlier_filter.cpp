// See https://en.cppreference.com/w/cpp/ranges
// TODO: Noet yet part of gcc-9
// See https://stackoverflow.com/questions/56118941/do-we-have-c20-ranges-library-in-gcc-9
//#include <ranges>
// Using boost as long as ranges lib made it into the std.
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>

#include "include/throw_if.h"
#include "include/outlier_filter.h"


OutlierFilterT::OutlierFilterT() : mOutlierBoundary(-1) {
}

void OutlierFilterT::setOutlierBoundary(float outlierBoundary) {
	mOutlierBoundary = outlierBoundary;
}

bool OutlierFilterT::isSet() const {
	return (mOutlierBoundary >= 0);
}

std::vector<PointWithResidualT> OutlierFilterT::getOutliers() const {
	return mOutliers;
}
