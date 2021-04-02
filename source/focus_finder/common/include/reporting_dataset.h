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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_REPORTING_DATASET_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_REPORTING_DATASET_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_REPORTING_DATASET_H_

#include <string>
#include <fstream>
#include <ctime>


class ReportingDatasetT {
private:
    std::string mSender;
    std::string mTitle;
    std::string mDetails;
    std::time_t mTime{};

public:
    ReportingDatasetT();

    ReportingDatasetT(std::string sender, std::string title, std::string details);

    [[nodiscard]] std::time_t getTime() const;

    [[nodiscard]] std::string getSender() const;

    [[nodiscard]] std::string getTitle() const;

    [[nodiscard]] std::string getDetails() const;

    friend std::ostream &operator<<(std::ostream &os, const ReportingDatasetT &reportingDataset);
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_REPORTING_DATASET_H_ */
