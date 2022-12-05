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

#ifndef FOFI_INTENSITY_WEIGHTED_CENTROID_ALGORITHM_H
#define FOFI_INTENSITY_WEIGHTED_CENTROID_ALGORITHM_H

#include <optional>

#include "centroid_algorithm.h"
#include "image.h"

class IntensityWeightedCentroidAlgorithmT : public CentroidAlgorithmT {
public:
    [[nodiscard]] std::string getName() const override;

    /**
     * The "Intensity Weighted Centroiding" is described on page 170 of
     * "Topics in Adaptive Optics" - "Advanced Methods for Improving the Efficiency
     * of a Shack Hartmann Wavefront Sensor"
     * See http://cdn.intechopen.com/pdfs-wm/26716.pdf
     *
     * @param inImg
     * @return Calculated centroid. NOTE: The position is in image coordinates.

     */
    [[nodiscard]] std::optional<PointT<float>> calc(const ImageT &inImg) const override;

    static PointT<float> calcIntensityWeightedCenter(const ImageT &inImg);
};

#endif //FOFI_INTENSITY_WEIGHTED_CENTROID_ALGORITHM_H
