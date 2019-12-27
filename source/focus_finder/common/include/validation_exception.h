#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_VALIDATION_EXCEPTION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_VALIDATION_EXCEPTION_H_

#include <string>
#include <exception>

// TODO: Maybe later add details about the validation problem...
class ValidationExceptionT: public std::exception {
public:
	ValidationExceptionT(const std::string & inMsg = "") :
			mName("ValidationExceptionT"), mMsg(inMsg) {
	}
	~ValidationExceptionT() throw () {
	}
	const char * what() const throw () {
		return mMsg.c_str();
	}
	const char * getName() const throw () {
		return mName.c_str();
	}

private:
	std::string mName;
	std::string mMsg;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_VALIDATION_EXCEPTION_H_*/
