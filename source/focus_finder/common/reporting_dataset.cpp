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

#include <chrono>
#include <ctime>
#include <utility>

#include "include/reporting_dataset.h"

ReportingDatasetT::ReportingDatasetT() = default;

ReportingDatasetT::ReportingDatasetT(std::string sender,
                                     std::string title, std::string details) :
        mSender(std::move(sender)), mTitle(std::move(title)), mDetails(std::move(details)) {

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

std::ostream &operator<<(std::ostream &os,
                         const ReportingDatasetT &reportingDataset) {
    os << std::ctime(&reportingDataset.mTime) << "   Sender: "
       << reportingDataset.mSender << ", Title: "
       << reportingDataset.mTitle << ", Details: "
       << reportingDataset.mDetails;

    return os;
}
