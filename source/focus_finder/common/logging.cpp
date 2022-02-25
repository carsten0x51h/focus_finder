/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c [at] lost-infinity.com>
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

#include "include/logging.h"

void LoggingT::init(const logging::trivial::severity_level &inLogSev, bool inWantConsoleLog, bool inWantLogFile) {
    if (inWantConsoleLog) {
        logging::add_console_log(
                std::cout,
                keywords::format = "[%TimeStamp%]: %Message%", /*< log record format >*/
                keywords::auto_flush = true
        );
    }

    if (inWantLogFile) {
        logging::add_file_log
                (
                        keywords::file_name = "sample_%N.log",  // file name pattern
                        keywords::rotation_size =
                                10 * 1024 * 1024,                    // rotate files every 10 MiB...
                        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0,
                                                                                            0), // ...or at midnight
                        keywords::format = "[%TimeStamp%]: %Message%",                            // log record format
                        keywords::auto_flush = true
                );
    }

    logging::core::get()->set_filter(logging::trivial::severity >= inLogSev);

    // TODO: Required? What exactly does this function?!
    logging::add_common_attributes();
}
