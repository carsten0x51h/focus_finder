/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_REPORTING_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_REPORTING_H_

#include <boost/signals2.hpp>

#include "reporting_dataset.h"
#include "logging.h"

class ReportingT {
private:
	// TODO: Maybe also pass a severity?
	typedef boost::signals2::signal<void (const ReportingDatasetT &)> NewReportListenersT;

	static NewReportListenersT sNewReportListeners;

public:
	static void reportMsg(const ReportingDatasetT & reportingDataset);

	static boost::signals2::connection registerNewReportListener(const NewReportListenersT::slot_type & inCallBack) {
		return sNewReportListeners.connect(inCallBack);
	}
	template <class T> static void unregisterNewReportListener(const T & inCallBack) {
		inCallBack.disconnect();
	}

protected:
	// NOTE: boost::signals2::signal is thread safe.
	// See https://theboostcpplibraries.com/boost.signals2-multithreading
	static void notifyNewReport(const ReportingDatasetT & reportingDataset) { sNewReportListeners(reportingDataset); }

private:
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_REPORTING_H_ */

