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

#include <optional>

#include "include/point.h"
#include "include/logging.h"
#include "include/intensity_weighted_centroid_algorithm.h"

std::string IntensityWeightedCentroidAlgorithmT::getName() const { return "IntensityWeightedCentroidAlgorithmT"; }

std::optional<PointT<float>> IntensityWeightedCentroidAlgorithmT::calc(const ImageT &inImg, const BackgroundThresholdFunctionT& inBgThresholdFunction) const {

    LOG(debug) << "IntensityWeightedCentroidAlgorithmT::calc..." << std::endl;

    if (inImg.width() <= 0 || inImg.height() <= 0) {
        throw CentroidExceptionT("No image supplied.");
    }

    // Apply background threshold function, if specified
    // In case the background threshold function is not set, this could be implemented
    // more efficient, but this is not likely and this way it is more readable.
    return std::optional<PointT<float> >{
        calcIntensityWeightedCenter(
                applyBackgroundThresholdFunction(inImg, inBgThresholdFunction)
        )
    };
}

ImageT IntensityWeightedCentroidAlgorithmT::applyBackgroundThresholdFunction(const ImageT &inImg, const BackgroundThresholdFunctionT &inBgThresholdFunction) {

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

bool IntensityWeightedCentroidAlgorithmT::isBackgroundThresholdFunctionSet(
        const CentroidAlgorithmT::BackgroundThresholdFunctionT &inBgThresholdFunction) { return inBgThresholdFunction != nullptr; }

PointT<float> IntensityWeightedCentroidAlgorithmT::calcIntensityWeightedCenter(const ImageT &inImg) {

    PointT<float> centroid;

    double I2;
    double I2x = 0;
    double I2y = 0;
    double Ixy2 = 0;

    // NOTE: indices x and y fun from 0..M-1.
    // However, for the calculation 1..M is expected.
    // Therefore, +1 is needed. Otherwise, the first I2x
    // and I2y are not counted.
    // TODO: Question is, if this is sufficient, or if from the result 1 must be subtracted again!
    cimg_forXY(inImg, x, y) {
        I2 = pow(inImg(x, y), 2.0);
        I2x += I2 * (x + 1);
        I2y += I2 * (y + 1);
        Ixy2 += I2;
    }

    // NOTE: Since the index of an image runs from 0..M-1,
    //       1 needs to be subtracted again.
    centroid.setX((float) (I2x / Ixy2) - 1);
    centroid.setY((float) (I2y / Ixy2) - 1);

    LOG(debug) << "IntensityWeightedCentroidAlgorithmT::calcIntensityWeightedCenter - Calculated centroid image pos=("
              << centroid.x() << ", " << centroid.y() << ")..."
              << std::endl;

    return centroid;
}
