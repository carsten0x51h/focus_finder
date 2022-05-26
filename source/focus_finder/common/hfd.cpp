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

#include "include/hfd.h"
#include "include/image.h"
#include <cmath>

const unsigned int HfdT::outerHfdDiameter = 55; // TODO: Calc?! - depends on pixel size and focal length (and seeing...) WAS 21!!! TODO: At least make this configurable - set from the outside!
const double HfdT::scaleFactor = 1.0;
const HfdT::BackgroundThresholdFunctionT HfdT::defaultBgThresholdFunction = [](const ImageT & img, unsigned bitDepth)-> double {
    return img.mean();
};

/**
 * TODO: Maybe this function should be removed, completely... It is a bit strange to have this kind of
 *       behaviour modelled in here...
 *
 * NOTE: This function assumes that the star center is in the center of the image.
 * NOTE: For even image width / height an inaccuracy is introduced here because there
 *       is no real center pixel.
 *
 * @param inImage
 * @param inOuterDiameter
 * @param inScaleFactor
 * @param outCenteredImg
 * @param inSubBgLevel
 * @return
 */
double HfdT::calculate(const ImageT &inImage, unsigned int inOuterDiameterPx,
                       double inScaleFactor, ImageT *outCenteredImg, const BackgroundThresholdFunctionT& inBgThresholdFunction) {

    PointT<unsigned int> starCenterPx((inImage.width() % 2 != 0 ? (inImage.width() - 1) : inImage.width())  / 2,
                                      (inImage.height() % 2 != 0 ? (inImage.height() - 1) : inImage.height())  / 2);

    return calculate(inImage, starCenterPx, inOuterDiameterPx, inScaleFactor, outCenteredImg, inBgThresholdFunction);
}

// TODO: As template.... float | double...?
double HfdT::calculate(const ImageT &inImage, const PointT<unsigned int> & starCenterPx, unsigned int inOuterDiameterPx,
                      double inScaleFactor, ImageT *outCenteredImg, const BackgroundThresholdFunctionT& inBgThresholdFunction) {

    if (inImage.is_empty()) {
        throw HfdExceptionT("Empty image supplied.");
    }

    // Crop the part from the supplied image which is needed to calculate the HFD value.
    // A boundary check is performed.
    RectT<int> imageBounds (0,0,inImage.width(), inImage.height());
    SizeT<unsigned int> subImageSize(inOuterDiameterPx, inOuterDiameterPx);

    // NOTE: Needs to be int because subImageRect x and/or y may be negative
    RectT<int> subImageRect = RectT<int>::fromCenterPoint(starCenterPx.to<int>(), subImageSize.to<int>());

    bool subImageRectInsideBounds = imageBounds.contains(subImageRect);

    if (! subImageRectInsideBounds) {
        std::stringstream ss;
        ss << "Cannot calculate HFD. Rect '" << subImageRect << "' defined by given starCenter '" << starCenterPx
           << "' is outside image bounds '" << imageBounds << "'." << std::endl;

        throw HfdExceptionT(ss.str());
    }

    // Finally, make a copy of the part of the image which is used to calculate the HFD.
    ImageT subImg = inImage.get_crop((int) subImageRect.x() /*x0*/,
                                     (int) subImageRect.y() /*y0*/,
                                     (int) (subImageRect.x() + subImageRect.width() - 1) /*x1*/,
                                     (int) (subImageRect.y() + subImageRect.height() - 1) /*y1*/);

    // Sub background threshold from image if threshold function is given
    if (inBgThresholdFunction != nullptr) {
        auto bgTh = (float) inBgThresholdFunction(subImg, 16 /*bit depth - TODO: Do not hardcode...*/);

        cimg_forXY(subImg, x, y) {
                subImg(x, y) = (subImg(x, y) < bgTh ? 0 : subImg(x, y) - bgTh);
            }
    }

    /**
     * interpolation_type - see https://cimg.eu/reference/structcimg__library_1_1CImg.html
     *
     * -1 = no interpolation: raw memory resizing.
     *  0 = no interpolation: additional space is filled according to boundary_conditions.
     *  1 = nearest-neighbor interpolation.
     *  2 = moving average interpolation.
     *  3 = linear interpolation.
     *  4 = grid interpolation.
     *  5 = cubic interpolation.
     *  6 = lanczos interpolation.
     */
    subImg.resize((int) (inScaleFactor * (double) subImg.width()),
                  (int) (inScaleFactor * (double) subImg.height()),
                  -100 /*size_z*/, -100 /*size_c*/,
                 1 /*interpolation_type*/);

    // Sum up all pixel values in whole circle
    PointT<double> scaledStarCenterSubImagePx((double) subImg.width() / 2.0, (double) subImg.height() / 2.0);
    double scaledOuterRadius = inScaleFactor * inOuterDiameterPx / 2.0;
    double sumPixelValues = 0;
    double sumWeightedDist = 0;

    cimg_forXY(subImg, x, y) {
        double pixelDistanceToCenter = std::hypot(x + 0.5 - scaledStarCenterSubImagePx.x(), y + 0.5 - scaledStarCenterSubImagePx.y());
        bool isInsideCircle = (pixelDistanceToCenter <= scaledOuterRadius);

        if (isInsideCircle) {
            sumPixelValues += subImg(x, y);
            sumWeightedDist += subImg(x, y) * pixelDistanceToCenter;
        }
    }

    // Make a copy of the image part which was used for calculation
    if (outCenteredImg != nullptr) {
        *outCenteredImg = subImg;
    }

    // NOTE: Multiplying with 2 is required since actually just the HFR is calculated above
    // NOTE: One exception is the case when there is no flux at all (i.e. a totally black image).
    //       In that case the HFD actually does not exist since there would be a division by 0.
    //       Therefore, in that situation NaN is returned.
    return (sumPixelValues > 0.0 ? (2.0 * sumWeightedDist / sumPixelValues) / inScaleFactor : NAN);
}

std::ostream &operator<<(std::ostream &os, const HfdT &hfd) {
    return hfd.print(os);
}

std::ostream &
HfdT::print(std::ostream &os) const {
    os << "HFD=" << mHfdValue << ", outer diameter=" << mOuterDiameter << ", max HFD limit: " << this->getMaxHfdLimit()
       << std::endl;
    return os;
}
