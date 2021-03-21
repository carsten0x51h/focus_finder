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

#include <boost/property_tree/ptree.hpp>

#include "include/focus_finder_logic.h"
#include "include/profile_manager.h"
#include "include/curve_fit_parms.h"
#include "include/focus_finder_calibration.h"
#include "include/focus_curve_record_set.h"
#include "include/focus_curve_record_set_container.h"
#include "include/focus_curve.h"
#include "include/logging.h"

FocusFinderCalibrationT::FocusFinderCalibrationT() : mFocusCurve(nullptr) {
    // TODO: Do we want to pass "const FocusCurveRecordSetContainerT & focusCurveRecordSetContainer" directly?
    LOG(debug) << "FocusFinderCalibrationT::FocusFinderCalibrationT - Focus curve record sets: "
               << mFocusCurveRecordSetContainer.size() << std::endl;
}

FocusFinderCalibrationT::~FocusFinderCalibrationT() {
    LOG(debug) << "FocusFinderCalibrationT::~FocusFinderCalibrationT - Focus curve record sets: "
               << mFocusCurveRecordSetContainer.size() << std::endl;
    // TODO: Do we want to pass "const FocusCurveRecordSetContainerT & focusCurveRecordSetContainer" directly?
}


FocusCurveRecordSetContainerT &FocusFinderCalibrationT::getCalibrationData() {
    return mFocusCurveRecordSetContainer;
}

// NOTE: Called from load() below...
void FocusFinderCalibrationT::setCalibrationData(const FocusCurveRecordSetContainerT &focusCurveRecordSetContainer,
                                                 const CurveFitParmsT &curveFitParms) {
    LOG(debug) << "FocusFinderCalibrationT::setCalibrationData - Focus curve record sets (old): "
               << mFocusCurveRecordSetContainer.size() << ", new - focusCurveRecordSetContainer: "
               << focusCurveRecordSetContainer.size() << std::endl;

    mFocusCurveRecordSetContainer = focusCurveRecordSetContainer;

    /**
     * Problem: There are N focus curves (one per record set). We need to calcuate the "master" focus curve.
     *
     * Ideas to calculate the master curve:
     *
     * Option 1
     *
     * Calculate average of all focus curve parameter -> This does not work for all curve types.
     * Simply averaging the parameters (e.g. of a hyperbolic curve) does not give the correct
     * "averaged" curve -. i.e. when adding all Hi(x) for i=0..N and divide by N. Therefore this
     * approach does not work here.
     *
     *
     * Option 2
     *
     * Calculate Hi(x) for all x between the relevant focus range (x is element N, i.e. integer
     * since there is no focus position 1.5). Then add all Hi(x) and divide by N for each x.
     * This will give the correct average (x, y) values. Note that for this approach parameter
     * c has to be set to 0. From those values either a LUT can be created or again a FocusCurveT
     * is created which automatically calculates the corresponding curve parameters.
     *
     *
     * Option 3
     *
     * Calculate just one "master" curve from all the data points from all record sets.
     * This is not a good idea since for each curve the the focus positions (x) can vary
     * - e.g. because of backlash of the focus or because of a focus position reset between
     * the recording of the different curve record sets.
     *
     *
     * Option 4
     *
     * Simplify the tool to only work with one single focus curve recorded.
     * In fact that might work. The improvement of having 3, 4, 5 or more focus curves
     * to further reduce the fluctuation and thus the uncertainty might not be so big
     * that it justifies the effort. Option 4 can be tested but to know how much better
     * Option 2 is, it also needs to be implemented.
     *
     *
     * Conclusion
     *
     * Option 4 will be implemented first - i.e. the mFocusCurve member will only hold
     * the FocusCurveT object for the first FocusCurveRecordSetT from the
     * mFocusCurveRecordSetContainer - the others will be ignored with a WARNING.
     * If that works perfectly well, there is no need for Option 2, and the whole recording
     * procedure may even be simplified. Otherwise, Option 2 will be implemented.
     */
    // TODO: Re-calculate the FocusCurveT!


    if (!mFocusCurveRecordSetContainer.empty()) {

        LOG(warning) << "!! Currently the focus curve only takes the first record set into account !!" << std::endl;

        // TODO / HACK / NOTE: Read comment above.
        auto firstFocusCurveRecord = mFocusCurveRecordSetContainer.front();

        mFocusCurve = std::make_shared<FocusCurveT>(firstFocusCurveRecord, curveFitParms);
    }
}

size_t FocusFinderCalibrationT::numRecordSets() const {
    return mFocusCurveRecordSetContainer.size();
}


bool FocusFinderCalibrationT::isValid() const {
    return (!mFocusCurveRecordSetContainer.empty() && mFocusCurve != nullptr);
}


std::shared_ptr<FocusCurveT> FocusFinderCalibrationT::getFocusCurve() const {
    return mFocusCurve;
}


void FocusFinderCalibrationT::save(boost::property_tree::ptree &pt,
                                   std::shared_ptr<FocusFinderCalibrationT> focusFinderCalibration,
                                   const std::filesystem::path &lightFramePath) {
    LOG(debug) << "FocusFinderCalibrationT::save..." << std::endl;

    boost::property_tree::ptree focusCurvesPt;
    const FocusCurveRecordSetContainerT &focusCurveRecordSets = focusFinderCalibration->getCalibrationData();

    // Clean the calibration/light_frame directory which holds the star images of the previous calibration (if any)
    LOG (debug) << "Removing calibration lightframes from " << lightFramePath.string() << "..." << std::endl;

    std::error_code errCode;
    std::uintmax_t numObjRemoved = std::filesystem::remove_all(lightFramePath, errCode);

    if (errCode.value() == 0) {
        LOG(debug) << "Removed " << numObjRemoved << " lightframes from '" << lightFramePath.string() << "'."
                   << std::endl;
    } else {
        LOG(error) << "Removing lightframes from '" << lightFramePath.string() << "' failed. Reason: "
                   << errCode.message() << std::endl;
        // TODO: throw!
    }

    // Re-create the light_frames directory
    std::filesystem::create_directories(lightFramePath, errCode);

    if (errCode.value() == 0) {
        LOG(debug) << "Created directory '" << lightFramePath.string() << "'." << std::endl;
    } else {
        LOG(error) << "Creating directory '" << lightFramePath.string() << "' failed." << std::endl;
        // TODO: throw!?
    }

    for (auto &focusCurveRecordSet : focusCurveRecordSets) {
        FocusCurveRecordSetT::save(focusCurvesPt, focusCurveRecordSet, lightFramePath);
    }

    pt.add_child("focus_curves", focusCurvesPt);

    LOG(debug) << "Finished storing '" << focusCurveRecordSets.size() << "' focus curve record sets..." << std::endl;
}

std::shared_ptr<FocusFinderCalibrationT>
FocusFinderCalibrationT::load(const boost::property_tree::ptree &pt, const CurveFitParmsT &curveFitParms,
                              const std::filesystem::path &lightFramePath) {
    LOG(debug) << "FocusFinderCalibrationT::load..." << std::endl;

    // Read all elements of a RecordSet...
    // See https://stackoverflow.com/questions/40393765/accessing-multi-valued-keys-in-property-tree
    // Also see https://akrzemi1.wordpress.com/2011/07/13/parsing-xml-with-boost/
    const auto &focusCurvesNode = pt.get_child("focus_curves");
    FocusCurveRecordSetContainerT focusCurveRecordSets;

    for (auto &focusCurveRecordSetPt : focusCurvesNode) {
        focusCurveRecordSets.push_back(FocusCurveRecordSetT::load(focusCurveRecordSetPt.second, lightFramePath));
    }

    LOG(debug) << "Finished loading '" << focusCurveRecordSets.size() << "' focus curve record sets..." << std::endl;

    auto focusFinderCalibration = std::make_shared<FocusFinderCalibrationT>();
    focusFinderCalibration->setCalibrationData(focusCurveRecordSets, curveFitParms);

    return focusFinderCalibration;
}

std::ostream &FocusFinderCalibrationT::print(std::ostream &os, size_t indent) const {
    std::string prefix = std::string(indent, ' ');

    // TODO: Print focus curve and record sets
    os << prefix << "Focus finder calibration" << std::endl
       << prefix << "Record sets" << std::endl;

    return mFocusCurveRecordSetContainer.print(os, indent + 4);
}

std::ostream &operator<<(std::ostream &os, const FocusFinderCalibrationT &focusFinderCalibration) {
    return focusFinderCalibration.print(os);
}
