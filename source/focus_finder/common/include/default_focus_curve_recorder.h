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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEFAULT_FOCUS_CURVE_RECORDER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEFAULT_FOCUS_CURVE_RECORDER_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEFAULT_FOCUS_CURVE_RECORDER_H_

#include "focus_curve_recorder.h"
#include "focus_measure_type.h"
#include "curve_sector.h"

// TODO / IDEA: Maybe HfdT and FwhmT should both implement a generic inteface "FocusMeasureT" which just hast "float getValue()".

class FocusCurveRecordT;

class FocusCurveRecordSetT;

class FocusControllerT;

class DefaultFocusCurveRecorderT : public FocusCurveRecorderT {
private:
    // Prevent copy
    DefaultFocusCurveRecorderT(const DefaultFocusCurveRecorderT &);

    DefaultFocusCurveRecorderT &operator=(const DefaultFocusCurveRecorderT &);

    FocusMeasureTypeT::TypeE getLimitFocusMeasureType() const;

    FocusMeasureTypeT::TypeE getCurveFocusMeasureType() const;

    float getFocusMeasureLimit() const;

    int getStepSize() const;

    //void cleanup();
    void checkCancelled() const;

    CurveSectorT::TypeE locateStartingPosition();

    std::shared_ptr<FocusCurveRecordSetT> recordFocusCurveRecordSet(CurveSectorT::TypeE curveHalf);

    void onFocusControllerProgressUpdate(float progress, const std::string &msg,
                                         std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr);

    void onFocusControllerNewRecord(std::shared_ptr<FocusCurveRecordT> focusCurveRecord);

    std::atomic<bool> mCancelled; // TODO: Still required?
    std::atomic<bool> mIsRunning;

    std::shared_ptr<FocusCurveRecordSetContainerT> mFocusCurveRecordSets;

public:
    DefaultFocusCurveRecorderT(std::shared_ptr<FocusControllerT> focusController);

    // Implement focus curve recorder interface
    std::string getName() const override;

    bool isRunning() const override;

    void run() override;

    void cancel() override;

    void reset() override;

    std::shared_ptr<const FocusCurveRecordSetContainerT> getFocusCurveRecordSets() const override;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEFAULT_FOCUS_CURVE_RECORDER_H_*/
