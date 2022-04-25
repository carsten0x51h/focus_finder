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
 * Get all pixels inside a radius: http://stackoverflow.com/questions/14487322/get-all-pixel-array-inside-circle
 * Algorithm:                      http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
 * HDF calculation:                http://www005.upp.so-net.ne.jp/k_miyash/occ02/halffluxdiameter/halffluxdiameter_en.html
 *                                 http://www.cyanogen.com/help/maximdl/Half-Flux.htm
 *
 * TODO: Add link to documentation and "Properties of HFD" blog entry.
 * TODO: Change default of inBgThresholdFunction to a good default thresholder.
 * TODO: Add LOG msgs to HfdT.
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_HFD_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_HFD_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_HFD_H_

#include "image.h"
#include "point.h"
#include "centroid.h"
#include "exception.h"

DEF_Exception(Hfd);

class HfdT {
public:
    typedef std::function<double(const ImageT&, unsigned int)> BackgroundThresholdFunctionT;

private:
    ImageT mImg;
    double mHfdValue{};
    unsigned int mOuterDiameter{};
    static const BackgroundThresholdFunctionT defaultBgThresholdFunction;

public:
    // NOTE: There is a bug in centerPosToFrame() which does not calculate the frame correct! Putting a 31 here hence leads to an exception...
    //       Fixing this bug leads to another problem where the center of the star does not seem to be calculated correctly any longer - the
    //       cross is shifted to the top-left in most of the cases.
    // NOTE: 27 gives better results with simulator! - Anyhow, 21 could be much better with real telescope.... TEST...
    static const unsigned int outerHfdDiameter; // TODO: Calc?! - depends on pixel size and focal length (and seeing...) WAS 21!!!
    static const double scaleFactor;

    HfdT() :
            mHfdValue(0), mOuterDiameter(outerHfdDiameter) {
    }

    // TODO / IDEA: Extend HFT class so that instead of "inSubBgLevel" a lambda threshold function can be passed in optionally.
    //              If none is specified, either a default is used or none at all (TBD)...

    explicit HfdT(const ImageT &inImage,
         unsigned int inOuterDiameter = outerHfdDiameter, double inScaleFactor = scaleFactor,
         BackgroundThresholdFunctionT inBgThresholdFunction = defaultBgThresholdFunction) {
        this->set(inImage, inOuterDiameter, inScaleFactor, inBgThresholdFunction);
    }

    explicit HfdT(const ImageT &inImage, const PointT<unsigned int> & starCenterPx,
                  unsigned int inOuterDiameter = outerHfdDiameter, double inScaleFactor = scaleFactor,
                  BackgroundThresholdFunctionT inBgThresholdFunction = defaultBgThresholdFunction) {
        this->set(inImage, starCenterPx, inOuterDiameter, inScaleFactor, inBgThresholdFunction);
    }

    inline void set(const ImageT &inImage, unsigned int inOuterDiameter =
    outerHfdDiameter, double inScaleFactor = scaleFactor, BackgroundThresholdFunctionT inBgThresholdFunction = defaultBgThresholdFunction) {
        mHfdValue = HfdT::calculate(inImage, inOuterDiameter, inScaleFactor, &mImg, inBgThresholdFunction);
        mOuterDiameter = inOuterDiameter;
    }

    inline void set(const ImageT &inImage, const PointT<unsigned int> & starCenterPx, unsigned int inOuterDiameter =
    outerHfdDiameter, double inScaleFactor = scaleFactor, BackgroundThresholdFunctionT inBgThresholdFunction = defaultBgThresholdFunction) {
        mHfdValue = HfdT::calculate(inImage, starCenterPx, inOuterDiameter, inScaleFactor, &mImg, inBgThresholdFunction);
        mOuterDiameter = inOuterDiameter;
    }

    static double calculate(const ImageT &inImage, unsigned int inOuterDiameter =
    outerHfdDiameter, double inScaleFactor = scaleFactor, ImageT *outCenteredImg = nullptr,
                            BackgroundThresholdFunctionT inBgThresholdFunction = defaultBgThresholdFunction);

    static double calculate(const ImageT &inImage, const PointT<unsigned int> & starCenterPx, unsigned int inOuterDiameter =
    outerHfdDiameter, double inScaleFactor = scaleFactor, ImageT *outCenteredImg = nullptr,
                            BackgroundThresholdFunctionT inBgThresholdFunction = defaultBgThresholdFunction);

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
