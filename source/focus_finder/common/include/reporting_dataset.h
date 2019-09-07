/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_REPORTING_DATASET_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_REPORTING_DATASET_H_

#include <string>
#include <fstream>
#include <ctime>


class ReportingDatasetT {
private:
	std::string mSender;
	std::string mTitle;
	std::string mDetails;
	std::time_t mTime;

public:
	ReportingDatasetT();
	ReportingDatasetT(const std::string & sender, const std::string & title, const std::string & details);

	std::time_t getTime() const;
	std::string getSender() const;
	std::string getTitle() const;
	std::string getDetails() const;

	friend std::ostream& operator<<(std::ostream& os, const ReportingDatasetT& reportingDataset);
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_REPORTING_DATASET_H_ */
