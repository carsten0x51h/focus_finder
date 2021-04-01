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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_REPORTING_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_REPORTING_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_REPORTING_H_

#include <boost/signals2.hpp>

#include "reporting_dataset.h"
#include "logging.h"

class ReportingT {
private:
    // TODO: Maybe also pass a severity?
    typedef boost::signals2::signal<void(const ReportingDatasetT &)> NewReportListenersT;

    static NewReportListenersT sNewReportListeners;

public:
    static void reportMsg(const ReportingDatasetT &reportingDataset);

    static boost::signals2::connection registerNewReportListener(const NewReportListenersT::slot_type &inCallBack) {
        return sNewReportListeners.connect(inCallBack);
    }

    template<class T>
    static void unregisterNewReportListener(const T &inCallBack) {
        inCallBack.disconnect();
    }

protected:
    // NOTE: boost::signals2::signal is thread safe.
    // See https://theboostcpplibraries.com/boost.signals2-multithreading
    static void notifyNewReport(const ReportingDatasetT &reportingDataset) { sNewReportListeners(reportingDataset); }

private:
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_REPORTING_H_ */

