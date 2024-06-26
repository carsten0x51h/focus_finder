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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_H_

#include "fwhm.h"
#include "hfd.h"
#include "image.h"
#include "point.h"
#include "focus_measure_type.h"
#include "exception.h"
#include "timestamp.h"
#include "filesystem_wrapper.h"

// TODO: Remove, when store() function has been extracted...
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

DEF_Exception(FocusCurveRecord);

class FocusCurveRecordT : public std::enable_shared_from_this<FocusCurveRecordT> {
private:
    TimestampT mCreationTimestamp;
    int mCurrentAbsoluteFocusPos;
    std::tuple<float, float> mDrift;
    std::chrono::duration<float> mExposureTime;

    float mSnr;
    FwhmT mFwhmHorz;
    FwhmT mFwhmVert;
    HfdT mHfd;
    ImageT mCorrectedStarImage;

public:
    FocusCurveRecordT(TimestampT creationTimestamp, int currentAbsoluteFocusPos,
                      std::chrono::duration<float> exposureTime, float snr,
                      FwhmT fwhmHorz, FwhmT fwhmVert, HfdT hfd,
                      ImageT correctedStarImage, std::tuple<float, float> drift);

    TimestampT getCreationTimestamp() const;

    int getCurrentAbsoluteFocusPos() const;

    std::chrono::duration<float> getExposureTime() const;

    // TODO: Do not store those as members but only calculate if required ... based on correctedStarImage...?
    // -> Even store "focus measure" in curve record??
    float getSnr() const;

    const FwhmT &getFwhmHorz() const;

    const FwhmT &getFwhmVert() const;

    const HfdT &getHfd() const;

    const ImageT &getCorrectedStarImage() const;

    const std::tuple<float, float> &getDrift() const;

    static float
    getFocusMeasure(const std::shared_ptr<FocusCurveRecordT>& focusCurveRecord, FocusMeasureTypeT::TypeE focusMeasureType);

    float getFocusMeasure(FocusMeasureTypeT::TypeE focusMeasureType);

    std::ostream &print(std::ostream &os, size_t indent = 0) const;

    friend std::ostream &operator<<(std::ostream &os, const FocusCurveRecordT &record);


    // TODO: Those functions may be moved out of this class because the dependency to property_tree shoud not be in here... It does not have to be a class member at all!
    static std::shared_ptr<FocusCurveRecordT>
    load(const boost::property_tree::ptree &pt, const fs::path &lightFramePath);

    static void save(boost::property_tree::ptree &pt, const FocusCurveRecordT &focusCurveRecord,
                     const fs::path &lightFramePath);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_H_*/
