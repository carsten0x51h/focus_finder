/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c.schmitt51h@gmail.com>
 *
 *  More info on https://www.lost-infinity.com
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

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

