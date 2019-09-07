#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_OUTLIER_FILTER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_OUTLIER_FILTER_H_

#include <vector>

#include "logging.h"
#include "exception.h"
#include "point_with_residual.h"

DEF_Exception(OutlierFilter);

class OutlierFilterT {
private:
	std::vector<PointWithResidualT> mOutliers;
	float mOutlierBoundary;

public:
	OutlierFilterT();

	bool isSet() const;
	void setOutlierBoundary(float outlierBoundary);
  template <typename RangeType>
  std::vector<PointFT> filter(const RangeType & pointsWithResiduals) {

	THROW_IF(OutlierFilter, mOutlierBoundary < 0, "outlierBoundary not set!");

	mOutliers.clear();
	
	size_t numInitialDataPoints = std::distance(pointsWithResiduals.begin(), pointsWithResiduals.end());
	
	std::vector<PointFT> matchedDataPoints;
	
	auto greaterThanBoundaryFunction = [&] (const auto & pointWithResidual) { return pointWithResidual.residual > mOutlierBoundary; };
	auto notGreaterThanBoundaryFunction = [&] (const auto & pointWithResidual) { return pointWithResidual.residual <= mOutlierBoundary; };

	// TODO: Is there a better way? Like collectAs?
	// TODO: Maybe better use boost::push-back?
	boost::range::copy(pointsWithResiduals | boost::adaptors::filtered(greaterThanBoundaryFunction), std::back_inserter(mOutliers));
	
	// TODO: Is there a better way? Like collectAs?
	boost::range::copy(pointsWithResiduals | boost::adaptors::filtered(notGreaterThanBoundaryFunction) | boost::adaptors::transformed([](const auto & pointWithResidual){ return pointWithResidual.point; }), std::back_inserter(matchedDataPoints));

	size_t numDataPointsRemoved = numInitialDataPoints - matchedDataPoints.size();
	
	LOG(debug) << "OutlierFilterT::filter() - matchedDataPoints.size(): " << matchedDataPoints.size() << ", removed: " << numDataPointsRemoved << "..."<< std::endl;
	
	return matchedDataPoints;
}


  std::vector<PointWithResidualT> getOutliers() const;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_OUTLIER_FILTER_H_*/
