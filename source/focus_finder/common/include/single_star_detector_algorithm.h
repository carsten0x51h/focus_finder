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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SINGLE_STAR_DETECTOR_ALGORITHM_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SINGLE_STAR_DETECTOR_ALGORITHM_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SINGLE_STAR_DETECTOR_ALGORITHM_H_

#include <memory>

#include "point.h"
#include "exception.h"
#include "image.h"
#include "size.h"

DEF_Exception(SingleStarDetector);

class SingleStarDetectorAlgorithmT {

private:
    float mSnrLimit;
    SizeT<unsigned int> mStarWindowSize;

public:
    /**
     * Result of algorithm modeled as inner class.
     * The properties mStarCenterPos and mSnr are only populated
     * under certain conditions. If not populated, mSnr and mSnrLimit
     * are set to 0.0F, mNumStarsDetected is set to -1
     * and mStarCenterPos is nullopt.
     */
    class ResultT {
    public:
        struct StatusT {
            enum TypeE {
                NO_INPUT_IMAGE_SET,
                STAR_WINDOW_OUT_OF_BOUNDS,
                NO_STAR_FOUND_SNR_TOO_LOW,
                CENTROID_CALC_FAILED,
                UNEXPECTED_STAR_COUNT,
                SINGLE_STAR_DETECTED,
                _Count
            };

            static const char *asStr(const TypeE &inType);
        };

    public:
        ResultT(StatusT::TypeE status, float snr = 0.0F, float snrLimit = 0.0F, int numStarsDetected = -1, const PointT<float> & starCenterPos.... add set functions.... solves this problem...);

        /**
         * Returns the status of the single star detector
         * @return
         */
        [[nodiscard]] StatusT::TypeE getStatus() const;


        // TODO: Maybe add a getDetailedDescription() method? E.g. how many stars were detected? What went wrong?...

        /**
         * @return In case no single star was detected, the SNR of the area around
         * the given poi (area size is defined by the star window size) or, the SNR
         * of the detected star image is returned.
         */
        [[nodiscard]] float getSnr() const;

        /**
         * @return Returns the SNR limit which was applied during the analysis.
         */
        [[nodiscard]] float getSnrLimit() const;

        /**
         * @return Returns the amount of detected stars.
         */
        [[nodiscard]] int getNumStarsDetected() const;

        /**
         * @return Returns star center position in image coordinates.
         */
        std::optional<PointT<float> > getStarCenterPos() const;

    private:
        StatusT::TypeE mStatus;
        float mSnr;
        float mSnrLimit;
        int mNumStarsDetected;
        std::optional<PointT<float> > mStarCenterPos;
    };


    /**
     *
     * @param snrLimit
     * @param starWindowSize
     */
    explicit SingleStarDetectorAlgorithmT(float snrLimit, const SizeT<unsigned int> & starWindowSize);

    /**
     *
     * @param image
     * @param poi - in image coordinates
     * @return ResultT structure
     */
    ResultT detect(std::shared_ptr<const ImageT> inImage, const PointT<float> & inPoi) const;

};

#endif // SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SINGLE_STAR_DETECTOR_ALGORITHM_H_
