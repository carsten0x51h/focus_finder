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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TMPL_ENUM_PTREE_TRANSLATOR_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TMPL_ENUM_PTREE_TRANSLATOR_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TMPL_ENUM_PTREE_TRANSLATOR_H_

#include "validation_exception.h"

#include <boost/property_tree/ptree.hpp>

template<typename T>
struct TmplEnumTranslatorT {
    typedef std::string internal_type;
    typedef typename T::TypeE external_type;

    boost::optional<external_type> get_value(const internal_type &str) {
        if (!str.empty()) {
            typename T::TypeE t = T::asType(str.c_str());

            if (t == T::_Count) {
                std::stringstream ss;
                ss << "Invalid enum value: '" << str << "'" << std::endl;
                throw ValidationExceptionT(ss.str());
            }

            return boost::optional<external_type>(t);
        } else {
            return boost::optional<external_type>(boost::none);
        }
    }

    boost::optional<internal_type> put_value(const external_type &b) {
        return boost::optional<internal_type>(T::asStr(b));
    }
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TMPL_ENUM_PTREE_TRANSLATOR_H_*/
