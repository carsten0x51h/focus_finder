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

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>

#include "../../common/include/centroid_algorithm_factory.h"
#include "../../common/include/thresholding_algorithm_factory.h"

BOOST_AUTO_TEST_SUITE(real_world_star_image_tests)

/**
 * Calculate centroid of a noisy image with a real star using the intensity weighted
 * centroiding method and the Max-Entropy background threshold function.
 *
 * Visually, the centroid is close to (x,y)=(15, 14).
 * Calculation using IWC gives (x,y) = (14.8288746, 13.9050961).
 */
BOOST_AUTO_TEST_CASE(intensity_weighted_centroid_max_entropy_threshold_real_noisy_star_test)
{
    ImageT realNoisyStarImage("test_data/test_image_23.tif");

    auto centroidAlgorithm = CentroidAlgorithmFactoryT::getInstance(CentroidAlgorithmTypeT::IWC);

    auto maxEntropyThresholdAlgorithm = ThresholdingAlgorithmFactoryT::getInstance(ThresholdingAlgorithmTypeT::MAX_ENTROPY);
    float threshold = maxEntropyThresholdAlgorithm->calc(realNoisyStarImage, 16);

    ImageT imgMinusThreshold(realNoisyStarImage, "xy");

    cimg_forXY(realNoisyStarImage, x, y) {
            float currIntensityValue = realNoisyStarImage(x, y);
            imgMinusThreshold(x,y) = (currIntensityValue > threshold ? currIntensityValue - threshold : 0);
        }

    std::optional<PointT<float>> centroidOpt = centroidAlgorithm->calc(imgMinusThreshold);

    BOOST_CHECK(centroidOpt.has_value());
    BOOST_CHECK_CLOSE(centroidOpt.value().x(), 14.8288746, 0.001F);
    BOOST_CHECK_CLOSE(centroidOpt.value().y(), 13.9050961, 0.001F);
}

BOOST_AUTO_TEST_SUITE_END();


