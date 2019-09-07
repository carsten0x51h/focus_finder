#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CURVE_FIT_EXCEPTION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CURVE_FIT_EXCEPTION_H_

#include <string>
#include <exception>

#include "curve_fit_summary.h"

class CurveFitExceptionT: public std::exception {
public:
	CurveFitExceptionT(const std::string & inMsg = "") :
			mName("CurveFitException"), mMsg(inMsg) {
	}
	CurveFitExceptionT(const std::string & inMsg,
			const CurveFitSummaryT & inSummary) :
			mName("CurveFitException"), mMsg(inMsg), mSummary(inSummary) {
	}
	~CurveFitExceptionT() throw () {
	}
	const char * what() const throw () {
		return mMsg.c_str();
	}
	const char * getName() const throw () {
		return mName.c_str();
	}
	const CurveFitSummaryT & getSummary() const throw () {
		return mSummary;
	}

private:
	std::string mName;
	std::string mMsg;
	CurveFitSummaryT mSummary;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CURVE_FIT_EXCEPTION_H_*/
