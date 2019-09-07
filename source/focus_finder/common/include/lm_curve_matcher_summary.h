#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_CURVE_MATCHER_SUMMARY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_CURVE_MATCHER_SUMMARY_H_

#include <ostream>
#include <string>

class LmCurveMatcherSummaryT {
public:
	int statusCode;
	std::string statusMsg;
	bool success;					// < Indicates if curve parameters were found under given constraints
	size_t numIterationsRequired;	// < Number of required iterations

    friend std::ostream & operator<<(std::ostream & os, const LmCurveMatcherSummaryT & summary);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_CURVE_MATCHER_SUMMARY_H_*/
