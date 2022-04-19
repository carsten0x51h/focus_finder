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

const float HfdT::outerHfdDiameter = 27.0F; // TODO: Calc?! - depends on pixel size and focal length (and seeing...) WAS 21!!! TODO: At least make this configurable - set from the outside!
const float HfdT::scaleFactor = 10.0F;

float HfdT::calculate(const ImageT &inImage, float inOuterDiameter,
                      float inScaleFactor, ImageT *outCenteredImg, bool inSubBgLevel) {

    if (inImage.is_empty()) {
        throw HfdExceptionT("Empty image supplied.");
    }

    // TODO: Should be calculated on sub-pixel level or the image should be re-sampled to a higher resolution.

    // TODO: Is this ok here???
    // Noise reduction
    // AD noise reduction --> In: Loaded image, Out: Noise reduced image
    // http://cimg.sourceforge.net/reference/structcimg__library_1_1CImg.html
//    ImageT aiImg = inImage.get_blur_anisotropic(130.0F, /*amplitude*/
//                                                0.7F, /*sharpness*/
//                                                0.3F, /*anisotropy*/
//                                                0.6F, /*alpha*/
//                                                1.1F, /*sigma*/
//                                                0.8F, /*dl*/
//                                                30, /*da*/
//                                                2, /*gauss_prec*/
//                                                0, /*interpolation_type*/
//                                                false /*fast_approx*/
//    );

    // TODO: Should the noise reduction step above be in this class?
    ImageT aiImg(inImage);


    // TODO: According to the definition of the HFD the "background level" of the star image should be subtracted - not the mean!
    // Sub mean image if desired
//    if (inSubBgLevel) {
//        auto mean = (float) aiImg.mean();
//
//        cimg_forXY(aiImg, x, y) {
//                aiImg(x, y) = (aiImg(x, y) < mean ? 0 : aiImg(x, y) - mean);
//            }
//    }

    // TODO: For some reason a cale factor > 40 decreases accuracy...
    aiImg.resize((int) (inScaleFactor * (float) aiImg.width()),
                 (int) (inScaleFactor * (float) aiImg.height()), -100 /*size_z*/, -100 /*size_c*/,
                 1 /*interpolation_type*/);

    // Sum up all pixel values in whole circle
    float scaledOuterRadius = inScaleFactor * inOuterDiameter / 2.0F;
    float sumPixelValues = 0;
    float sumDist = 0;

    // NOTE: This implementation assumes that the star center is in the center of the image.
    // TODO: An extension would be to create another calculate() method which allows to supply
    //       the center position.
    // TODO: This is for the ODD case... Handle EVEN case... Does it make any difference at all?
    int starCentroidIdxX = (aiImg.width() - 1) / 2;
    int starCentroidIdxY = (aiImg.height() - 1) / 2;

    // One pixel has the size 1x1. The center of the pixel is reached by adding (0.5, ß.5).
    float starCentroidPosX = (float) starCentroidIdxX + 0.5F;
    float starCentroidPosY = (float) starCentroidIdxY + 0.5F;

    cimg_forXY(aiImg, x, y) {
        float pixelDistanceToCenter = calcPixelDistanceToCenter((float) x + 0.5F, (float) y + 0.5F, starCentroidPosX /*centerX*/, starCentroidPosY /*centerY*/);
        bool isInsideCircle = (pixelDistanceToCenter <= scaledOuterRadius);

        if (isInsideCircle) {
            sumPixelValues += aiImg(x, y);
            sumDist += aiImg(x, y) * pixelDistanceToCenter;
        }
    }

    // Make a copy of the image part which was used for calculation
    if (outCenteredImg != nullptr) {
        *outCenteredImg = aiImg;
    }

    // NOTE: Multiplying with 2 is required since actually just the HFR is calculated above
    // NOTE: One exception is the case when there is no flux at all (i.e. a totally black image).
    //       In that case the HFD actually does not exist since there would be a division by 0.
    //       Therefore, in that situation NaN is returned.
    return (sumPixelValues > 0.0F ? (2.0F * sumDist / sumPixelValues) / inScaleFactor : NAN);
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

float HfdT::calcPixelDistanceToCenter(float inX, float inY, float inCenterX, float inCenterY) {
    return std::hypot(inX - inCenterX, inY - inCenterY);
}
