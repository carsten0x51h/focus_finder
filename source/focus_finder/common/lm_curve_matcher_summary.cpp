#include <ostream>

#include "include/lm_curve_matcher_summary.h"

std::ostream & operator<<(std::ostream & os,
		const LmCurveMatcherSummaryT & lmCurveMatcherSummary) {
	static const char * prefix = "   > ";

	os << "** LmCurveMatcherSummary **" << std::endl;
	os << prefix << "success: " << lmCurveMatcherSummary.success << std::endl
	   << prefix << "numIterationsRequired: " << lmCurveMatcherSummary.numIterationsRequired << std::endl
	   << prefix << "statusCode: " << lmCurveMatcherSummary.statusCode << std::endl
		<< prefix << "statusMsg: " << lmCurveMatcherSummary.statusMsg << std::endl;

	return os;
}
