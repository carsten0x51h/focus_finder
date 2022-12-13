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
#include <boost/test/data/test_case.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>

#include "../../common/include/hfd.h"
#include "../../common/include/centroid_algorithm_factory.h"
#include "../../common/include/thresholding_algorithm_factory.h"

// TODO: This "real world" file is a bucket for everything... it should be renamed...

namespace bdata = boost::unit_test::data;

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


/**
 * The HFD algorithm is executed on a set of real star images recorded with a Newtonian
 * telescope. The telescope focus is moved from "far out of focus" to "in focus". In sum
 * 11 images were recorded. The focus distance is equal between the images. The star
 * image never reaches saturation. The star center slightly moves what is ignored in this
 * test.
 */
BOOST_DATA_TEST_CASE(hfd_real_newton_focus_star_no_background_subtraction_test,
        bdata::make(
        std::vector< std::tuple<int, double> >{
                { 1, 27.456088096660295 },
                { 2, 26.039354380832002 },
                { 3, 24.798843695746001 },
                { 4, 22.743999207494756 },
                { 5, 19.926210044511915 },
                { 6, 17.341647262965008 },
                { 7, 14.616004395669748 },
                { 8, 12.516580080411764 },
                { 9, 10.090836864932291 },
                { 10, 7.0064759328706696 },
                { 11, 6.3337311227004358 }
        }),
        focusStarNumber, expectedFocusStarHfd)
{
    std::stringstream filenameSs;
    filenameSs << "test_data/hfd/newton_focus_star/newton_focus_star" << focusStarNumber << ".tiff";

    // For the unit-tests a very simple background threshold function is used - the mean() function.
    // NOTE: Instead the "Max entropy" or "Otsu" thresholding algorithm can be used.
    //
    // TODO: Change this by using a ThresholderAlgorithmT::... MEAN -> problem currently is, that HFD
    //       is doing a subimage() for a given "outer HFD radius" and then the threshold is calculated
    //       and subtracted. Hence, subtracting the mean here, before doing the subimage(), results in
    //       a different result!
    auto bgThresholdFunction = [](const ImageT & img, unsigned bitDepth)-> double {
        return img.mean();
    };

    BOOST_CHECK_CLOSE(
        HfdT::calculate(
                ImageT(filenameSs.str().c_str()),
                PointT<unsigned int> (33, 47), // Star center position manually extracted from first image
                55.0 /*outer diameter*/,
                1.0F /* scale factor */,
                nullptr,
                bgThresholdFunction
        ),
        expectedFocusStarHfd,
        0.01F
);
}

BOOST_AUTO_TEST_SUITE_END();
