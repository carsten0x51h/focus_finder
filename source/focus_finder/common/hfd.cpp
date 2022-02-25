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

const float HfdT::outerHfdDiameter = 27.0f; // TODO: Calc?! - depends on pixel size and focal length (and seeing...) WAS 21!!!

float HfdT::calc(const ImageT &inImage, float inOuterDiameter,
                 ImageT *outCenteredImg, bool inSubMean) {

    // TODO: Is this ok here???
    // Noise reduction
    // AD noise reduction --> In: Loaded image, Out: Noise reduced image
    // http://cimg.sourceforge.net/reference/structcimg__library_1_1CImg.html
    ImageT aiImg = inImage.get_blur_anisotropic(130.0F, /*amplitude*/
                                                0.7F, /*sharpness*/
                                                0.3F, /*anisotropy*/
                                                0.6F, /*alpha*/
                                                1.1F, /*sigma*/
                                                0.8F, /*dl*/
                                                30, /*da*/
                                                2, /*gauss_prec*/
                                                0, /*interpolation_type*/
                                                false /*fast_approx*/
    );

    // Sub mean image if desired
    if (inSubMean) {
        double mean = aiImg.mean();
        cimg_forXY(aiImg, x, y) {
                aiImg(x, y) = (aiImg(x, y) < mean ? 0 : aiImg(x, y) - mean);
            }
    }

    // TODO: Check that iScaleFactor is at least 1
    // TODO: Scale up image if necessary

    // Sum up all pixel values in whole circle
    float outerRadius = inOuterDiameter / 2.0F;
    float sum = 0;
    float sumDist = 0;

    cimg_forXY(aiImg, x, y) {
            if (insideCircle((float) x, (float) y, outerRadius /*centerX*/, outerRadius /*centerY*/,
                             outerRadius)) {
                sum += aiImg(x, y);
                sumDist += aiImg(x, y)
                           * std::sqrt(
                        std::pow((float) x - outerRadius /*centerX*/, 2.0F)
                        + pow((float) y - outerRadius /*centerX*/,
                              2.0F));
            }
        }

    // Make a copy of the image part which was used for calculation
    if (outCenteredImg != nullptr) {
        // TODO: Zoom image?
        *outCenteredImg = aiImg;
    }

    // NOTE: Multiplying with 2 is required since actually just the HFR is calculated above
    return (sum > 0 ? 2.0F * sumDist / sum : std::sqrt(2.0F) * outerRadius);
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

bool HfdT::insideCircle(float inX, float inY, float inCenterX, float inCenterY, float inRadius) {
    return (std::pow(inX - inCenterX, 2.0) + std::pow(inY - inCenterY, 2.0) <= std::pow(inRadius, 2.0));
}
