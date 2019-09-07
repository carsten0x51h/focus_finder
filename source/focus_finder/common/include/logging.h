/*****************************************************************************
 *
 *  Focus Finder
 *
 *  Copyright(C) 2015 Carsten Schmitt <c.schmitt51h@gmail.com>
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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LOGGING_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LOGGING_H_

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

#include <boost/log/sinks/text_ostream_backend.hpp>
//#include <boost/utility/empty_deleter.hpp>
#include <boost/log/utility/setup/console.hpp>

//#include <boost/regex.hpp>

#include <fstream>


namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
//namespace po = boost::program_options;

using namespace logging::trivial;


// See http://stackoverflow.com/questions/24170577/simultaneous-logging-to-console-and-file-using-boost
#define LOG(level) BOOST_LOG_SEV(global_logger::get(), logging::trivial::level)

typedef src::severity_channel_logger_mt<severity_level, std::string> global_logger_type;

BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(global_logger, global_logger_type)
{
  return global_logger_type(boost::log::keywords::channel = "global_logger");
}

// TODO: Do we need channels? See http://www.boost.org/doc/libs/1_54_0/libs/log/doc/html/log/detailed/sources.html
class LoggingT {
private:

public:
  static void init(const logging::trivial::severity_level & inLogSev = logging::trivial::debug, bool inWantConsoleLog = false, bool inWantLogFile = true);
};



#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LOGGING_H_ */
