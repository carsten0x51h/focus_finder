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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TIMESTAMP_PTREE_TRANSLATOR_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TIMESTAMP_PTREE_TRANSLATOR_H_

#include "timestamp.h"
#include "validation_exception.h"

#include <boost/property_tree/ptree.hpp>

struct TimestampTranslatorT {
    typedef std::string internal_type;
    typedef TimestampT external_type;

    boost::optional<external_type> get_value(const internal_type &str) {
        if (!str.empty()) {

            try {
                long tsLong = std::stol(str);

                std::chrono::milliseconds durationInMs{tsLong};
                TimestampT ts(durationInMs);

                return boost::optional<external_type>(ts);
            }
            catch (std::invalid_argument &exc) {
                std::stringstream ss;
                ss << "Invalid TimestampT value: '" << str << "'. Details: " << exc.what() << std::endl;
                throw ValidationExceptionT(ss.str());
            }
            catch (std::out_of_range &exc) {
                std::stringstream ss;
                ss << "Value '" << str << "'"" is out of range for TimestampT. Details: " << exc.what() << std::endl;
                throw ValidationExceptionT(ss.str());
            }
        } else {
            return boost::optional<external_type>(boost::none);
        }
    }

    boost::optional<internal_type> put_value(const external_type &timestamp) {
        std::stringstream ss;
        ss << std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()).count();

        return boost::optional<internal_type>(ss.str());
    }
};

/*  Specialize translator_between so that it uses our custom translator for
    bool value types. Specialization must be in boost::property_tree
    namespace. */
namespace boost {
    namespace property_tree {
        template<typename Ch, typename Traits, typename Alloc>
        struct translator_between<std::basic_string<Ch, Traits, Alloc>, TimestampT> {
            typedef TimestampTranslatorT type;
        };
    } // namespace property_tree
}

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TIMESTAMP_PTREE_TRANSLATOR_H_*/
