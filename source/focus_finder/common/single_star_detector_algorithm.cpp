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

#include "include/rect.h"
#include "include/snr.h"
#include "include/centroid.h"

#include "include/single_star_detector_algorithm.h"

/**
 * Result class
 */
const char * SingleStarDetectorAlgorithmT::ResultT::StatusT::asStr(const TypeE &inType) {
    switch (inType) {
        case NO_INPUT_IMAGE_SET:
            return "NO_INPUT_IMAGE_SET";
        case STAR_WINDOW_OUT_OF_BOUNDS:
            return "STAR_WINDOW_OUT_OF_BOUNDS";
        case NO_STAR_FOUND_SNR_TOO_LOW:
            return "NO_STAR_FOUND_SNR_TOO_LOW";
        case CENTROID_CALC_FAILED:
            return "CENTROID_CALC_FAILED";
        case UNEXPECTED_STAR_COUNT:
            return "UNEXPECTED_STAR_COUNT";
        case SINGLE_STAR_DETECTED:
            return "SINGLE_STAR_DETECTED";
        default:
            return "<?>";
    }
}

SingleStarDetectorAlgorithmT::ResultT::ResultT(StatusT::TypeE status, float snr, float snrLimit, int numStarsDetected, const PointT<float> & starCenterPos) : mStatus(status), mSnr(snr), mSnrLimit(snrLimit), mNumStarsDetected(numStarsDetected), mStarCenterPos(starCenterPos) {
}

SingleStarDetectorAlgorithmT::ResultT::StatusT::TypeE SingleStarDetectorAlgorithmT::ResultT::getStatus() const {
    return mStatus;
}

std::optional<PointT<float> > SingleStarDetectorAlgorithmT::ResultT::getStarCenterPos() const {
    return mStarCenterPos;
}

float SingleStarDetectorAlgorithmT::ResultT::getSnr() const {
    return mSnr;
}

float SingleStarDetectorAlgorithmT::ResultT::getSnrLimit() const {
    return mSnrLimit;
}

int SingleStarDetectorAlgorithmT::ResultT::getNumStarsDetected() const {
    return mNumStarsDetected;
}

SingleStarDetectorAlgorithmT::SingleStarDetectorAlgorithmT(float snrLimit, const SizeT<unsigned int> & starWindowSize) : mSnrLimit(snrLimit), mStarWindowSize(starWindowSize) {
}

SingleStarDetectorAlgorithmT::ResultT SingleStarDetectorAlgorithmT::detect(std::shared_ptr<const ImageT> inImage, const PointT<float> & inPoi) const {

    if (inImage == nullptr) {
        return ResultT{ResultT::StatusT::NO_INPUT_IMAGE_SET };
    }

    PointT<int> poi = inPoi.to<int>();

    RectT<int> imageBounds (0,0,inImage->width(), inImage->height());

    RectT<int> searchWindowRect = RectT<int>::fromCenterPoint(poi, mStarWindowSize.to<int>());

    LOG(debug) << "SingleStarDetectorAlgorithmT::detect... searchWindowRect: "
               << " x=" << searchWindowRect.x() << ", y=" << searchWindowRect.y()
               << ", w=" << searchWindowRect.width() << ", h=" << searchWindowRect.height()
               << std::endl;

    // Handle case that star is to close to the image boundary.
    bool selectionInsideBounds = imageBounds.contains(searchWindowRect);

    if (!selectionInsideBounds) {
        LOG(warning)
                << "SingleStarDetectorAlgorithmT::detect... searchWindow out of image bounds."
                << std::endl;

        return ResultT{ResultT::StatusT::STAR_WINDOW_OUT_OF_BOUNDS };
    }

    // NOTE: CImg doc - boundary_conditions = Can be { 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }.
    // 		 In mathematics, the Dirichlet (or first-type) boundary condition is a type of boundary condition,
    // 		 named after Peter Gustav Lejeune Dirichlet (1805–1859).
    //
    //		 See: https://en.wikipedia.org/wiki/Dirichlet_boundary_condition
    //
    // 		 In mathematics, the Neumann (or second-type) boundary condition is a type of boundary condition,
    //		 named after Carl Neumann.[1] When imposed on an ordinary or a partial differential equation, the
    //		 condition specifies the values in which the derivative of a solution is applied within the boundary
    //       of the domain.
    //
    // 		 See https://en.wikipedia.org/wiki/Neumann_boundary_condition
    ImageT searchWindowImg = inImage->get_crop(
            searchWindowRect.x() /*x0*/,
            searchWindowRect.y() /*y0*/,
            searchWindowRect.x() + searchWindowRect.height() - 1 /*x1*/,
            searchWindowRect.y() + searchWindowRect.width() - 1 /*y1*/);

    // Calculate the SNR to check if there COULD be a star around.
    auto snr = (float) SnrT::calculate(searchWindowImg);

    LOG(debug)
            << "SingleStarDetectorAlgorithmT::detect...SNR: " << snr << std::endl;

    if (snr < mSnrLimit) {
        LOG(warning)
                << "SingleStarDetectorAlgorithmT::detect... no valid star detected. SNR "
                << snr << " too low. SNR limit is " << mSnrLimit << "."
                << std::endl;

        return ResultT{ ResultT::StatusT::NO_STAR_FOUND_SNR_TOO_LOW, snr, mSnrLimit };
    }


    // Check that there is only a single star in the selected region
    StarCounterAlgorithmT starCounterAlgorithm(imageBitDepth, ThresholdingAlgorithmTypeT::MAX_ENTROPY);
    int numStarsDetected = starCounterAlgorithm.count(searchWindowImg); // const ImageT &inImg

    if (numStarsDetected != 1) {
        LOG(warning)
                << "SingleStarDetectorAlgorithmT::detect...  detected " << numStarsDetected << " star(s)"
                << " but expected exactly 1."
                << std::endl;

        return ResultT{ ResultT::StatusT::UNEXPECTED_STAR_COUNT, snr, mSnrLimit, numStarsDetected };
    }


    // Try to calculate the centroid of the star image
    //
    // TODO: In addition we may want to configure
    //		 -Centroid type
    //		 -subtract mean (?) - should this be configured??
    //       -starRect = size of the star rect / "window" (from profile cfg)
    //       -maxSearchRect = 2 or 3 * starRect - max search boundary (from profile cfg)
    // TODO: What to do with ImageT calcImage ? Do we need it at all?
    ImageT calcImage;
    auto calcCenterOpt = CentroidT::calculate(searchWindowImg,
                                              CentroidTypeT::IWC, true /*sub mean*/, &calcImage);

    // If there is a result, transform the returned coordinates (sub frame system)
    // back to the "window" coordinates (directly replace in the same optional object.
    if (calcCenterOpt) {
        calcCenterOpt.emplace(
                PointT<float>(calcCenterOpt->x() + (float) searchWindowRect.x(),
                              calcCenterOpt->y() + (float) searchWindowRect.y()));

        return ResultT{ ResultT::StatusT::SINGLE_STAR_DETECTED, snr, mSnrLimit, numStarsDetected, calcCenterOpt.value() };
    }
    else {
        // TODO: Maybe add set() methods to ResultT class... so no repetition is required all the time...
        return ResultT{ResultT::StatusT::CENTROID_CALC_FAILED, snr, mSnrLimit, numStarsDetected, calcCenterOpt };
    }
}

/** ---------------------------------------------------------------------------
 * Usage:
 * IN: poi in pixmap coordinates - i.e. where user clicked...
 *
 * auto activeProfile = mFoFiConfigManager->getProfileManager()->getActiveProfile();
 *
 * if (!activeProfile) {
 *   LOG(warning) << "No active profile set!" << std::endl;
 *   return std::nullopt;
 * }
 *
 * auto img = mLastFrame.getImage();
 *
 * if (img == nullptr) {
 *   LOG(warning)
 *      << "FocusFinderLogicT::findFocusStar... failed - no frame available."
 *      << std::endl;
 *   return std::nullopt;
 * }
 *
 *    // NOTE: Point "poi" passed in is in "pixmap" coordinates, i.e. in case there is
 *    //       just a subframe image available, the coordinates need to be transformed.
 *    //       In case roi is not set, roiX and roiY are just 0. That means we can make
 *    //       the same transformation always - just in case of a "full frame" it simply
 *    //       has no effect.
 *    auto frameBounds = mLastFrame.getBounds().to<int>(); // RectT<unsigned int>
 *
 *  // TODO: Coordinates are not 100% correct!!?? Needs debug / Check...
 *
 *  // Transform given POI to "sub-frame" coordinates:
 *  //
 *  // NOTE: Point "poi" passed in is in "pixmap" coordinates, i.e. in case there is
 *  //       just a subframe image available, the coordinates need to be transformed.
 *  //       In case roi is not set, roiX and roiY are just 0. That means we can make
 *  //       the same transformation always - just in case of a "full frame" it simply
 *  //       has no effect.
 *  auto frameBounds = mLastFrame.getBounds().to<int>(); // RectT<unsigned int>
 *
 *
 * SingleStarDetectorAlgorithmT singleStarDetectorAlgorithm(activeProfile->getStarDetectionSnrBoundary(), activeProfile->getStarWindowSize())
 *
 * Point<float> subFramePoi(poi.x() - frameBounds.x(), poi.y() - frameBounds.y());
 *
 * SingleStarDetectorAlgorithmT::ResultT result = singleStarDetectorAlgorithm.detect(img, subFramePoi);
 *
 * if (result.status == ResultT::StatusT::SINGLE_STAR_DETECTED) {
 *    // NOTE: In case of success, the calculated star center coordinate needs to be re-transformed to pixmap coordinates!
 *    Point<float> centerCoord(starCenter.x() + frameBounds.x(), starCenter.y() - frameBounds.y());
 *    ...
 * }
 */