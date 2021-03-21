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

/**
 *
 * Get all pixels inside a radius: http://stackoverflow.com/questions/14487322/get-all-pixel-array-inside-circle
 * Algorithm:                      http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
 * HDF calculation:                http://www005.upp.so-net.ne.jp/k_miyash/occ02/halffluxdiameter/halffluxdiameter_en.html
 *                                 http://www.cyanogen.com/help/maximdl/Half-Flux.htm
 *
 * NOTE: Currently the accuracy is limited by the insideCircle function (-> sub-pixel accuracy).
 * NOTE: The HFD is estimated in case there is no flux (HFD ~ sqrt(2) * inOuterDiameter / 2).
 * NOTE: The outer diameter is usually a value which depends on the properties of the optical
 *       system and also on the seeing conditions. The HFD value calculated depends on this
 *       outer diameter value.
 *
 * TODO: Implement scale up.
 * TODO: Add LOG msgs to HfdT.
 */

#ifndef _HFD_H_
#define _HFD_H_ _HFD_H_

#include "image.h"
#include "point.h"
#include "centroid.h"
#include "exception.h"

DEF_Exception(Hfd);

class HfdT {
private:
    ImageT mImg;
    float mHfdValue;
    unsigned int mOuterDiameter;

public:
// NOTE: There is a bug in centerPosToFrame() which does not calculate the frame correct! Putting a 31 here hence leads to an exception...
//       Fixing this bug leads to another problem where the center of the star does not seem to be calculated correctly any longer - the
//       cross is shifted to the top-left in most of the cases.
// NOTE: 27 gives better results with simulator! - Anyhow, 21 could be much better with real telescope.... TEST...
    static const unsigned int outerHfdDiameter = 27; // TODO: Calc?! - depends on pixel size and focal length (and seeing...) WAS 21!!!

    static bool
    insideCircle(float inX /*pos of x*/, float inY /*pos of y*/, float inCenterX, float inCenterY, float inRadius) {
        return (std::pow(inX - inCenterX, 2.0) + std::pow(inY - inCenterY, 2.0) <= std::pow(inRadius, 2.0));
    }


    HfdT() :
            mHfdValue(0), mOuterDiameter(outerHfdDiameter) {
    }

    HfdT(const ImageT &inImage,
         unsigned int inOuterDiameter = outerHfdDiameter, bool inSubMean =
    true) {
        this->set(inImage, inOuterDiameter, inSubMean);
    }

    inline void set(const ImageT &inImage, unsigned int inOuterDiameter =
    outerHfdDiameter, bool inSubMean = true) {
        mHfdValue = HfdT::calc(inImage, inOuterDiameter, &mImg, inSubMean);
        mOuterDiameter = inOuterDiameter;
    }

    static float calc(const ImageT &inImage, unsigned int inOuterDiameter =
    outerHfdDiameter, ImageT *outCenteredImg = 0,
                      bool inSubMean = true);

    inline bool valid() const {
        return (mHfdValue > 0 && mImg.width() > 0 && mImg.height() > 0);
    }

    inline void reset() {
        mHfdValue = 0;
        mImg.assign(); // In-place version of the default constructor CImg(). It simply resets the instance to an empty image.
    }

    inline float getValue() const {
        return mHfdValue;
    }

    inline const ImageT &getResultImage() const {
        return mImg;
    }

    inline float getOuterDiameter() const {
        return mOuterDiameter;
    }


    // NOTE: mOuterDiameter / (2.0 * sqrt(2))
    // TODO: Why 2*sqrt(2)?
    static float getMaxHfdLimit(float inOuterHfdDiameter) {
        return 0.353553390593 * inOuterHfdDiameter;
    }

    inline float getMaxHfdLimit() const {
        return HfdT::getMaxHfdLimit(mOuterDiameter);
    }

    std::ostream &print(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const HfdT &hfd);
};

#endif // _HFD_H_
