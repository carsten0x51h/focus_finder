#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_CURVE_MATCHER_EXCEPTION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_CURVE_MATCHER_EXCEPTION_H_

#include <string>
#include <exception>

#include "lm_curve_matcher_summary.h"

class LmCurveMatcherExceptionT : public std::exception {
public:
	LmCurveMatcherExceptionT(const std::string & inMsg = "") : mName("LmCurveMatcherException"), mMsg(inMsg) { }
	LmCurveMatcherExceptionT(const std::string & inMsg, const LmCurveMatcherSummaryT & inSummary) : mName("LmCurveMatcherException"), mMsg(inMsg), mSummary(inSummary) { }
	~LmCurveMatcherExceptionT() throw() {}
	const char * what() const throw() { return mMsg.c_str(); }
	const char * getName() const throw() { return mName.c_str(); }
	const LmCurveMatcherSummaryT & getSummary() const throw() { return mSummary; }

private:
  std::string mName;
  std::string mMsg;
  LmCurveMatcherSummaryT mSummary;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_CURVE_MATCHER_EXCEPTION_H_*/
