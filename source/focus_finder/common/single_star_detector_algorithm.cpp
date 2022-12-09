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

#include "include/thresholding_algorithm_factory.h"
#include "include/thresholding_algorithm.h"
#include "include/star_cluster_algorithm.h"

/**
 * Result class
 */
const char * SingleStarDetectorAlgorithmT::ResultT::StatusT::asStr(const TypeE &inType) {
    switch (inType) {
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

SingleStarDetectorAlgorithmT::ResultT::ResultT() : mStatus(StatusT::_Count), mSnr(0.0F), mSnrLimit(0.0F), mNumStarsDetected(0), mStarCenterPos(PointT<float>(-1.0F, -1.0F)) {
}

SingleStarDetectorAlgorithmT::ResultT::StatusT::TypeE SingleStarDetectorAlgorithmT::ResultT::getStatus() const {
    return ResultT::mStatus;
}

void SingleStarDetectorAlgorithmT::ResultT::setStatus(SingleStarDetectorAlgorithmT::ResultT::StatusT::TypeE status) {
    ResultT::mStatus = status;
}

PointT<float> SingleStarDetectorAlgorithmT::ResultT::getStarCenterPos() const {
    return ResultT::mStarCenterPos;
}

void SingleStarDetectorAlgorithmT::ResultT::setStarCenterPos(const PointT<float> &starCenterPos) {
    ResultT::mStarCenterPos = starCenterPos;
}

float SingleStarDetectorAlgorithmT::ResultT::getSnr() const {
    return ResultT::mSnr;
}

void SingleStarDetectorAlgorithmT::ResultT::setSnr(float snr) {
    ResultT::mSnr = snr;
}

float SingleStarDetectorAlgorithmT::ResultT::getSnrLimit() const {
    return ResultT::mSnrLimit;
}

void SingleStarDetectorAlgorithmT::ResultT::setSnrLimit(float snrLimit) {
    ResultT::mSnrLimit = snrLimit;
}

unsigned int SingleStarDetectorAlgorithmT::ResultT::getNumStarsDetected() const {
    return ResultT::mNumStarsDetected;
}

void SingleStarDetectorAlgorithmT::ResultT::setNumStarsDetected(unsigned int numStarsDetected) {
    ResultT::mNumStarsDetected = numStarsDetected;
}

// TODO: operator<<...


std::ostream &
SingleStarDetectorAlgorithmT::ResultT::print(std::ostream &os, size_t indent) const {
    std::string prefix = std::string(indent, ' ');

    os << prefix << "status: " << SingleStarDetectorAlgorithmT::ResultT::StatusT::asStr(mStatus)
       << prefix << ", SNR: " << mSnr
       << prefix << ", SNR limit: " << mSnrLimit
       << prefix << ", Star center pos: " << mStarCenterPos
       << prefix << ", numStarsDetected: " << mNumStarsDetected
       << std::endl;

    return os;
}

std::ostream &operator<<(std::ostream &os,
                         const SingleStarDetectorAlgorithmT::ResultT &result) {
    return result.print(os);
}






SingleStarDetectorAlgorithmT::SingleStarDetectorAlgorithmT(float snrLimit, const SizeT<unsigned int> & starWindowSize) : mSnrLimit(snrLimit), mStarWindowSize(starWindowSize) {
}

unsigned int SingleStarDetectorAlgorithmT::calcNumStarsInRegion(const ImageT &inImg) {

    auto thAlgo = ThresholdingAlgorithmFactoryT::getInstance(ThresholdingAlgorithmTypeT::MAX_ENTROPY);
    const static size_t BIT_DEPTH = 16; // TODO: Do not hardcode - but where to query??
    float th = thAlgo->calc(inImg, BIT_DEPTH /*bit depth */);
    int thUp = std::ceil(th);

    LOG(debug) << "SingleStarDetectorAlgorithmT::calcNumStarsInRegion - threshold: " << th << ", thUp: " << thUp << std::endl;

    ImageT binaryImg = inImg.get_threshold((float) thUp + 1.0F /*HACK!*/); // Threshold function somehow uses >=

    StarClusterAlgorithmT starClusterAlgorithm(
            2 /*defines the allowed number of dark pixels between two white pixels until they form a cluster*/);
    std::list<PixelClusterT> clusters = starClusterAlgorithm.cluster(binaryImg);

    LOG(debug) << "SingleStarDetectorAlgorithmT::calcNumStarsInRegion - Found " << clusters.size() << " stars..." << std::endl;

    return clusters.size();
}


SingleStarDetectorAlgorithmT::ResultT SingleStarDetectorAlgorithmT::detect(std::shared_ptr<const ImageT> inImage, const PointT<float> & inPoi) const {

    ResultT result;

    result.setSnrLimit(mSnrLimit);

    if (inImage == nullptr) {
        throw SingleStarDetectorExceptionT("No input image set.");
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

        result.setStatus(ResultT::StatusT::STAR_WINDOW_OUT_OF_BOUNDS);
        return result;
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

    result.setSnr(snr);

    LOG(debug)
            << "SingleStarDetectorAlgorithmT::detect...SNR: " << snr << std::endl;

    if (snr < mSnrLimit) {
        LOG(warning)
                << "SingleStarDetectorAlgorithmT::detect... no valid star detected. SNR "
                << snr << " too low. SNR limit is " << mSnrLimit << "."
                << std::endl;

        result.setStatus(ResultT::StatusT::NO_STAR_FOUND_SNR_TOO_LOW);

        return result;
    }


    // Check that there is only a single star in the selected region
    unsigned int numStarsDetected = calcNumStarsInRegion(searchWindowImg);

    result.setNumStarsDetected(numStarsDetected);

    if (numStarsDetected != 1) {
        LOG(warning)
                << "SingleStarDetectorAlgorithmT::detect...  detected " << numStarsDetected << " star(s)"
                << " but expected exactly 1."
                << std::endl;

        result.setStatus(ResultT::StatusT::UNEXPECTED_STAR_COUNT);

        return result;
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
                PointT<float> transformedPoint(calcCenterOpt->x() + (float) searchWindowRect.x(),
                              calcCenterOpt->y() + (float) searchWindowRect.y());

        result.setStatus(ResultT::StatusT::SINGLE_STAR_DETECTED);
        result.setStarCenterPos(transformedPoint);
    }
    else {
        result.setStatus(ResultT::StatusT::CENTROID_CALC_FAILED);
    }
    return result;
}
