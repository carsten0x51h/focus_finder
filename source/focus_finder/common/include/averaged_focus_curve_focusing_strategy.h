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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_AVERAGED_FOCUS_CURVE_FOCUSING_STRATEGY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_AVERAGED_FOCUS_CURVE_FOCUSING_STRATEGY_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_AVERAGED_FOCUS_CURVE_FOCUSING_STRATEGY_H_

#include <vector>
#include <chrono>
#include <boost/signals2.hpp>

#include "focus_finder.h"
#include "curve_fit_algorithm.h"
#include "focus_controller.h"
#include "boundary_location.h"
#include "focus_interface.h"

class CurveFunctionT;

///////////////////////////////////////////////////////////////////////
// Custom data structure + accessor
///////////////////////////////////////////////////////////////////////
// TODO: Rename... My....
// TODO: Is this actually needed any more? + my DataAccessorT?
typedef std::pair<float, float> MyDataPointT;
typedef std::vector<MyDataPointT> MyDataContainerT;

class MyDataAccessorT {
public:
    typedef MyDataContainerT TypeT;

    static PointFT getDataPoint(size_t /*inIdx*/, TypeT::const_iterator inIt) {
        return PointFT(inIt->first /*inIdx*/, inIt->second /*y*/);
    }
};

class AveragedFocsCurveFocusingStrategyT : public FocusFinderT {
private:
    // Prevent copy
    AveragedFocsCurveFocusingStrategyT(const AveragedFocsCurveFocusingStrategyT &);

    AveragedFocsCurveFocusingStrategyT &operator=(const AveragedFocsCurveFocusingStrategyT &);

    //float estimateRelPos(std::shared_ptr<CurveFunctionT> focusCurveFunction, float focusMeasure, CurveSectorT::TypeE curveHalf);

    void rollbackFocus();

    void focusFinderCleanup();

    void checkCancelled() const;

    std::shared_ptr<const ImageT> mCurrentImage;

    boost::signals2::connection mCameraExposureFinishedConnection;

    std::atomic<bool> mCancelled;
    std::atomic<bool> mIsRunning;
    int mInitialAbsPosition{};

public:
    explicit AveragedFocsCurveFocusingStrategyT(std::shared_ptr<FocusControllerT> focusController);

    // Implement focus finder interface
    [[nodiscard]] std::string getName() const override;

    [[nodiscard]] bool isRunning() const override;

    void run() override;

    void cancel() override;

    void reset() override;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_AVERAGED_FOCUS_CURVE_FOCUSING_STRATEGY_H_ */
