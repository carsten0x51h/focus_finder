#include "include/logging.h"

void LoggingT::init(const logging::trivial::severity_level & inLogSev, bool inWantConsoleLog, bool inWantLogFile) {
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
			 keywords::file_name = "sample_%N.log",                                        /*< file name pattern >*/
			 keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
			 keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
			 keywords::format = "[%TimeStamp%]: %Message%",                                /*< log record format >*/
			 keywords::auto_flush = true
			);
    }

    logging::core::get()->set_filter(logging::trivial::severity >= inLogSev);

    // TODO: Required? What exactly does this function?!
    logging::add_common_attributes();
}
