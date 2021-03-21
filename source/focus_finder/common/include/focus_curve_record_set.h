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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORD_SET_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORD_SET_H_

#include <memory>
#include <fstream>
#include <vector>
#include <tuple>
#include <list>

#include "focus_curve_record.h"
#include "focus_measure_type.h"

#include "image.h"
#include "point.h"

class FocusCurveRecordSetT : public std::vector<std::shared_ptr<FocusCurveRecordT> > {
private:
    FocusMeasureTypeT::TypeE mFocusMeasureType;
    float mFocusMeasureLimit;

public:
    FocusCurveRecordSetT(FocusMeasureTypeT::TypeE focusMeasureType, float focusMeasureLimit);

    FocusMeasureTypeT::TypeE getFocusMeasureType() const;

    float getFocusMeasureLimit() const;

    std::pair<int, int> minmaxFocusPos() const;

    static std::shared_ptr<FocusCurveRecordSetT>
    load(const boost::property_tree::ptree &pt, const std::filesystem::path &lightFramePath);

    static void save(boost::property_tree::ptree &pt, std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet,
                     const std::filesystem::path &lightFramePath);

    std::ostream &print(std::ostream &os, size_t indent = 0) const;

    friend std::ostream &operator<<(std::ostream &os, const FocusCurveRecordSetT &focusCurveRecordSet);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORD_SET_H_*/




