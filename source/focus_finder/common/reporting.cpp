#include "include/reporting.h"
#include "include/logging.h"

ReportingT::NewReportListenersT ReportingT::sNewReportListeners;

// NOTE: boost::signals2::signal is thread safe.
// See https://theboostcpplibraries.com/boost.signals2-multithreading
void ReportingT::reportMsg(const ReportingDatasetT & reportingDataset) {
	notifyNewReport(reportingDataset);
}
