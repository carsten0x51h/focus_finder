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

#include "include/centroid_algorithm.h"

ImageT CentroidAlgorithmT::applyBackgroundThresholdFunction(const ImageT &inImg, const BackgroundThresholdFunctionT &inBgThresholdFunction) {

    if (isBackgroundThresholdFunctionSet(inBgThresholdFunction)) {
        double backgroundThreshold = inBgThresholdFunction(inImg);

        // Create an emtpy image with xy-dimensions of inImg.
        // See https://cimg.eu/reference/structcimg__library_1_1CImg.html
        ImageT starImage(inImg, "xy");

        // NOTE: If resulting pixel value is negative, 0 is applied.
        cimg_forXY(inImg, x, y) {
                starImage(x, y) = (float) (inImg(x, y) < backgroundThreshold ? 0 : inImg(x, y) - backgroundThreshold);
            }

        return starImage;
    }
    else {
        // Return a copy
        return inImg;
    }
}

bool CentroidAlgorithmT::isBackgroundThresholdFunctionSet(
        const BackgroundThresholdFunctionT &inBgThresholdFunction) { return inBgThresholdFunction != nullptr; }