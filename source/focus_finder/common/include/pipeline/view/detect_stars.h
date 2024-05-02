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

#ifndef STARMATH_DETECT_STARS_H
#define STARMATH_DETECT_STARS_H STARMATH_DETECT_STARS_H

#include <range/v3/view/transform.hpp>

#include "../../image.h"

#include "../../star_cluster_algorithm.h"
#include "../../thresholding_algorithm.h"


#define STARMATH_DETECT_STARS_DEBUG 0

namespace starmath::pipeline {

    template<typename ImageType=float>
    auto
    detect_stars(int clusterRadius, const std::shared_ptr<ThresholdingAlgorithmT> &thresholding_algorithm) {

        using SharedImageT = std::shared_ptr<cimg_library::CImg<ImageType>>;

    	return ranges::views::transform(
                [=](const SharedImageT &image) {

                    const auto &imageRef = *image;

                    DEBUG_IMAGE_DISPLAY(imageRef, "detect_stars_in", STARMATH_DETECT_STARS_DEBUG);

                    // TODO: Do not hardcode bit depth 16.... make it part of ImageT?
                    float threshold = std::ceil(thresholding_algorithm->calc(imageRef, 16));

                    // NOTE: CImg threshold function uses >=
                    auto binaryImg = imageRef.get_threshold(threshold + 1.0F);

                    DEBUG_IMAGE_DISPLAY(binaryImg, "detect_stars_image", STARMATH_DETECT_STARS_DEBUG);

                    StarClusterAlgorithmT starClusterAlgorithm(clusterRadius);
                    auto pixelClusters = starClusterAlgorithm.cluster(binaryImg);

                    auto rectsVec =
                    		pixelClusters
							| ranges::views::transform([=](const auto & pixelCluster) { return pixelCluster.getBounds(); })
							| ranges::to<std::vector>();

                    return std::make_pair(image, rectsVec);
                }
        );
    }
}

#endif //STARMATH_DETECT_STARS_H
