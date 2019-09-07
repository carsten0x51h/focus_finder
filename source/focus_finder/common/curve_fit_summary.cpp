#include <ostream>

#include "include/point_with_residual.h"
#include "include/curve_fit_summary.h"



	bool success;								// < Indicates if curve parameters were found under given constraints
	size_t numIterationsRequiredTotal = 0;		// < Number of required iterations in total
    float outlierBoundary;						// < Contains outlier boundary in case outlier detection was enabled, otherwise -1
    std::vector<PointWithResidualT> outliers;	// < Contains outliers in case outlier detection was enabled

std::ostream & operator<<(std::ostream & os, const CurveFitSummaryT & summary) {
  static const char * prefix = "   > ";

  os << "** CurveFitSummary **" << std::endl;

  //success
  os << prefix << "Curve fitting status" << std::endl;
  os << prefix << (success ? "SUCCESS" : "FAILURE") << std::endl;

  // matchedDataPoints
  os << prefix << "Matched data points [#dp=" << summary.matchedDataPoints.size() << "]" << std::endl;

  for (const auto & p : summary.matchedDataPoints) {
    os << prefix << p << std::endl;
  }

  // TODO: For some reason this does not compile..
  // std::copy(summary.matchedDataPoints.begin(),
  // 		summary.matchedDataPoints.end(),
  // 	    std::ostream_iterator<PointFT>(os));

  os << std::endl;


  // outliers
  bool outlierDetectionEnabled = outlierBoundary >= 0;

  if (outlierDetectionEnabled) {
	  os << prefix << "Outliers [#outliers=" << summary.outliers.size() << ", boundary=" << outlierBoundary << "]" << std::endl;
	  
	  for (const auto & pr : summary.outliers) {
	    os << prefix << pr << std::endl;
	  }
	  // TODO: For some reason that does not compile...
	  // std::copy(summary.outliers.begin(),
	  // 		  summary.outliers.end(),
	  // 		  std::ostream_iterator<PointWithResidualT>(os));

  } else {
	  os << prefix << "Outliers [detection disabled]" << std::endl;
  }
  os << std::endl;


  os << prefix << "Number of iterations total=" << numIterationsRequiredTotal << std::endl;

  return os;
}
