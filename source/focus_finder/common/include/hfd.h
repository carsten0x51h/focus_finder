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

/**
 * HDF calculation
 * https://www.lost-infinity.com/night-sky-image-processing-part-6-measuring-the-half-flux-diameter-hfd-of-a-star-a-simple-c-implementation/
 * https://www.lost-infinity.com/the-half-flux-diameter-hfd-for-a-perfectly-normal-distributed-star/
 * https://www.lost-infinity.com/the-half-flux-diameter-hfd-of-a-plain-image/
 * http://www005.upp.so-net.ne.jp/k_miyash/occ02/halffluxdiameter/halffluxdiameter_en.html
 * http://www.cyanogen.com/help/maximdl/Half-Flux.htm
 *
 * Get all pixels inside a radius
 * http://stackoverflow.com/questions/14487322/get-all-pixel-array-inside-circle
 *
 * TODO: Change further unit tests to DATA-TESTS..., rename / move image files
 * TODO: Rename all .tif to .tiff
 *
 * TODO: Change HFD so that:
 *    -Only quadratic images are accepted
 *    -Always set outerRadius to given image dimensions -> no subimage() call anymore
 *    -Assume that sar centroid is in the center of the image
 *    -Remove thresholder - assume that thresholding took place before
 *        --> Separation of concerns
 *
 *      FwhmT fwhm = starImg.fwhm(...)
 *      HfdT hfd = starImg.hfd(...)
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_HFD_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_HFD_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_HFD_H_

#include <utility>
#include <functional>

#include "image.h"
#include "point.h"
#include "exception.h"

DEF_Exception(Hfd);

class HfdT {
private:
    ImageT mImg;
    double mHfdValue{};
    unsigned int mOuterDiameter{};

public:
    static const unsigned int outerHfdDiameter;
    static const double scaleFactor;

    HfdT() :
            mHfdValue(0), mOuterDiameter(outerHfdDiameter) {
    }

    explicit HfdT(const ImageT &inImage,
         unsigned int inOuterDiameter = outerHfdDiameter, double inScaleFactor = scaleFactor) {
        this->set(inImage, inOuterDiameter, inScaleFactor);
    }

    explicit HfdT(const ImageT &inImage, const PointT<unsigned int> & starCenterPx,
                  unsigned int inOuterDiameter = outerHfdDiameter, double inScaleFactor = scaleFactor) {
        this->set(inImage, starCenterPx, inOuterDiameter, inScaleFactor);
    }

    inline void set(const ImageT &inImage, unsigned int inOuterDiameter =
    outerHfdDiameter, double inScaleFactor = scaleFactor) {
        mHfdValue = HfdT::calculate(inImage, inOuterDiameter, inScaleFactor, &mImg);
        mOuterDiameter = inOuterDiameter;
    }

    inline void set(const ImageT &inImage, const PointT<unsigned int> & starCenterPx, unsigned int inOuterDiameter =
    outerHfdDiameter, double inScaleFactor = scaleFactor) {
        mHfdValue = HfdT::calculate(inImage, starCenterPx, inOuterDiameter, inScaleFactor, &mImg);
        mOuterDiameter = inOuterDiameter;
    }

    static double calculate(const ImageT &inImage, unsigned int inOuterDiameter =
    outerHfdDiameter, double inScaleFactor = scaleFactor, ImageT *outCenteredImg = nullptr);

    static double calculate(const ImageT &inImage, const PointT<unsigned int> & starCenterPx, unsigned int inOuterDiameter =
    outerHfdDiameter, double inScaleFactor = scaleFactor, ImageT *outCenteredImg = nullptr);

    [[nodiscard]] inline bool valid() const {
        return (mHfdValue > 0 && mImg.width() > 0 && mImg.height() > 0);
    }

    inline void reset() {
        mHfdValue = 0;
        mImg.assign(); // In-place version of the default constructor CImg(). It simply resets the instance to an empty image.
    }

    [[nodiscard]] inline double getValue() const {
        return mHfdValue;
    }

    [[nodiscard]] inline const ImageT &getResultImage() const {
        return mImg;
    }

    [[nodiscard]] inline unsigned int getOuterDiameter() const {
        return mOuterDiameter;
    }


    // NOTE: mOuterDiameter / (2.0 * sqrt(2))
    // TODO: Why 2*sqrt(2)?
    static double getMaxHfdLimit(unsigned int inOuterHfdDiameter) {
        return 0.353553390593 * inOuterHfdDiameter;
    }

    [[nodiscard]] inline double getMaxHfdLimit() const {
        return HfdT::getMaxHfdLimit(mOuterDiameter);
    }

    std::ostream &print(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const HfdT &hfd);
};

#endif // SOURCE_FOCUS_FINDER_COMMON_INCLUDE_HFD_H_
