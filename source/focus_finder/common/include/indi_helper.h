/*
 * image.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_HELPER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_HELPER_H_

#include <string>

#include "exception.h"

#include "indiapi.h"
#include "basedevice.h"

DEF_Exception(Indi);

class IndiHelperT {
public:
	static std::string propPermAsStr(IPerm perm);
	static std::string propStateAsStr(IPState state);

	template<typename T>
	static void requireWritable(T * vec) {
		IPerm perm = vec->p;

		if (perm != IP_WO && perm != IP_RW) {
			std::stringstream ss;
			ss << "Property '" << vec->name
					<< "' is required to be writable but is '"
					<< IndiHelperT::propPermAsStr(perm) << "'.";

			throw IndiExceptionT(ss.str());
		}
	}

	template<typename T>
	static void requireReadable(T * vec) {
		IPerm perm = vec->p;

		if (perm != IP_RO && perm != IP_RW) {
			std::stringstream ss;
			ss << "Property '" << vec->name
					<< "' is required to be readable but is '"
					<< IndiHelperT::propPermAsStr(perm) << "'.";

			throw IndiExceptionT(ss.str());
		}
	}

	static INumberVectorProperty * getNumberVec(INDI::BaseDevice * dev,
			const std::string & vecPropName);
	static INumber * getNumber(INumberVectorProperty * vec,
			const std::string & propName);

	static ISwitchVectorProperty * getSwitchVec(INDI::BaseDevice * dev,
			const std::string & vecPropName);
	static ISwitch * getSwitch(ISwitchVectorProperty * vec,
			const std::string & propName);

	static ITextVectorProperty * getTextVec(INDI::BaseDevice * dev,
			const std::string & vecPropName);
	static IText * getText(ITextVectorProperty * vec,
			const std::string & propName);

	// TODO: Add further types? -> if needed...
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_HELPER_H_ */

