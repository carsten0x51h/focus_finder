#include <chrono>
#include <ctime>

#include "include/reporting_dataset.h"

ReportingDatasetT::ReportingDatasetT() {

}

ReportingDatasetT::ReportingDatasetT(const std::string & sender,
		const std::string & title, const std::string & details) :
		mSender(sender), mTitle(title), mDetails(details) {

	mTime = std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now());
}

std::time_t ReportingDatasetT::getTime() const {
	return mTime;
}

std::string ReportingDatasetT::getSender() const {
	return mSender;
}

std::string ReportingDatasetT::getTitle() const {
	return mTitle;
}

std::string ReportingDatasetT::getDetails() const {
	return mDetails;
}

std::ostream & operator<<(std::ostream & os,
		const ReportingDatasetT & reportingDataset) {
	os << std::ctime(&reportingDataset.mTime) << "   Sender: "
			<< reportingDataset.mSender << ", Title: "
			<< reportingDataset.mTitle << ", Details: "
			<< reportingDataset.mDetails;

	return os;
}
