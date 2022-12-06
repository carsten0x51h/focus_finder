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
#include "include/center_of_gravity_centroid_algorithm.h"

std::string CenterOfGravityCentroidAlgorithmT::getName() const { return "CenterOfGravityCentroidAlgorithmT"; }

std::optional<PointT<float>> CenterOfGravityCentroidAlgorithmT::calc(const ImageT &inImg) const {

    LOG(debug) << "CenterOfGravityCentroidAlgorithmT::calc..." << std::endl;

    if (inImg.width() <= 0 || inImg.height() <= 0) {
        throw CentroidExceptionT("No image supplied.");
    }

    // Apply background threshold function, if specified
    // In case the background threshold function is not set, this could be implemented
    // more efficient, but this is not likely and this way it is more readable.
    return std::optional<PointT<float> >{ calcCenterOfGravity(inImg) };
}

PointT<float> CenterOfGravityCentroidAlgorithmT::calcCenterOfGravity(const ImageT &inImg) {

    PointT<float> centroid;

    double Ix = 0;
    double Iy = 0;
    double sumOfAllIntensityValues = 0;

    // NOTE: indices x and y fun from 0..M-1.
    // However, for the calculation 1..M is expected.
    // Therefore, +1 is needed. Otherwise, the first I2x
    // and I2y are not counted.
    cimg_forXY(inImg, x, y) {
        Ix += inImg(x, y) * ((float) x + 1.0F);
        Iy += inImg(x, y) * ((float) y + 1.0F);
        sumOfAllIntensityValues += inImg(x, y);
    }

    // NOTE: Since the index of an image runs from 0..M-1,
    //       1 needs to be subtracted again.
    centroid.setX((float) (Ix / sumOfAllIntensityValues) - 1);
    centroid.setY((float) (Iy / sumOfAllIntensityValues) - 1);

    LOG(debug) << "CenterOfGravityCentroidAlgorithmT::calcCenterOfGravity - Calculated centroid image pos=("
               << centroid.x() << ", " << centroid.y() << ")..."
               << std::endl;

    return centroid;
}
