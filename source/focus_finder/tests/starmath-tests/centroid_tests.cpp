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

#include <optional>

#include "../../common/include/centroid_algorithm_factory.h"
#include "../../common/include/thresholding_algorithm_factory.h"
#include "../../common/include/image.h"
#include "../../common/include/point.h"

/**
 * TODO: Rename image files - Encode dimensions - e.g. 120x120 and type
 *       of image (e.g. plain/all_values_1/gaussian). Adapt names in HFD
 *       tests as well. Why? test_image_15.tif does not say anything.
 */
BOOST_AUTO_TEST_SUITE(centroid_tests)



/**
 * Calculate centroid of a black square image using the intensity weighted
 * centroiding method.
 *
 * The IWC value for a plain image can be easily calculated analytically.
 * Assuming a plain image where all pixel values are A, and the image
 * dimensions are M x M. Then, I2x = sum_ij(I_ij^2*x), I2y = sum_ij(I_ij^2*y)
 * and Ixy2 = sum_ij(I_ij^2) can be calculated as:
 *
 * I2x = I2y = M * A * \frac{M*(M+1)}{2}
 * Ixy2 = M^2 * A
 *
 * With this, the corresponding IWC can be calculated as:
 *
 * xc = yc = \frac{M+1}{2}
 *
 * Since the pixel index starts with 0, 1 needs to be subtracted.
 * That means, as a result, the expected image indices for a plain
 * image of pixel value A and dimension M x M are:
 *
 * xc_idx = yc_idx = \frac{M-1}{2}
 *
 * That means, the pixel A value has no effect n the result.
 * The expected centroid is exactly in the center of the image,
 *
 * Example:
 *
 * For a 5x5 image with pixel value 1, the expected calculated indices
 * for the image centroid are:
 *
 * xc_idx = yc_idx = \frac{5-1}{2} = \frac{4}{2} = 2
 *
 * For the 5x5 image (image index starting with 0,0), this is
 * exactly in the center.
 */
BOOST_AUTO_TEST_CASE(intensity_weighted_centroid_no_bg_threshold_all_pixel_values_equal_1_test)
{
    ImageT plainImage("test_data/test_image_22.tif"); // All pixels have the value 1 - 5x5

    auto centroidAlgorithm = CentroidAlgorithmFactoryT::getInstance(CentroidAlgorithmTypeT::IWC);

    std::optional<PointT<float>> centroidOpt = centroidAlgorithm->calc(plainImage);

    BOOST_CHECK(centroidOpt.has_value());
    BOOST_CHECK_EQUAL(centroidOpt.value(), PointT<float>(2.0F,2.0F));
}

/**
 * Calculate centroid of an image with one syntetic, perfect star using the intensity weighted
 * centroiding method.
 *
 * The expected centroid index position is (x,y)=(22,14).
 */
BOOST_AUTO_TEST_CASE(intensity_weighted_centroid_no_bg_threshold_perfect_star_test)
{
    ImageT perfectStarImage("test_data/test_image_21.tif"); // Ideal star with centroid at (x,y)=(22, 14)

    auto centroidAlgorithm = CentroidAlgorithmFactoryT::getInstance(CentroidAlgorithmTypeT::IWC);

    std::optional<PointT<float>> centroidOpt = centroidAlgorithm->calc(perfectStarImage);

    BOOST_CHECK(centroidOpt.has_value());
    BOOST_CHECK_EQUAL(centroidOpt.value(), PointT<float>(22.0F,14.0F));
}


/**
 * Calculate centroid of a noisy image with a real star using the intensity weighted
 * centroiding method without using a background threshold function.
 *
 * Visually, the centroid is close to (x,y)=(15, 14).
 * Calculation using IWC gives (x,y) = (13.1486, 13.6157).
 */
BOOST_AUTO_TEST_CASE(intensity_weighted_centroid_no_bg_threshold_real_noisy_star_test)
{
    ImageT perfectStarImage("test_data/test_image_23.tif");

    auto centroidAlgorithm = CentroidAlgorithmFactoryT::getInstance(CentroidAlgorithmTypeT::IWC);

    std::optional<PointT<float>> centroidOpt = centroidAlgorithm->calc(perfectStarImage);

    BOOST_CHECK(centroidOpt.has_value());
    BOOST_CHECK_CLOSE(centroidOpt.value().x(), 13.1486F, 0.001F);
    BOOST_CHECK_CLOSE(centroidOpt.value().y(), 13.6157F, 0.001F);
}


/**
 * Calculate centroid of a noisy image with a real star using the intensity weighted
 * centroiding method and the Max-Entropy background threshold function.
 *
 * Visually, the centroid is close to (x,y)=(15, 14).
 * Calculation using IWC gives (x,y) = (14.8288746, 13.9050961).
 */
BOOST_AUTO_TEST_CASE(intensity_weighted_centroid_max_entropy_threshold_real_noisy_star_test)
{
    ImageT perfectStarImage("test_data/test_image_23.tif");

    auto maxEntropyThresholdAlgorithm = ThresholdingAlgorithmFactoryT::getInstance(ThresholdingAlgorithmTypeT::MAX_ENTROPY);
    auto centroidAlgorithm = CentroidAlgorithmFactoryT::getInstance(CentroidAlgorithmTypeT::IWC);


    auto thresholdFunction = [=](const ImageT& inImg) -> double {
        // TODO: Need to find a solution for the bit-depth... -> make part of ImageT?
        return maxEntropyThresholdAlgorithm->calc(inImg, 16);
    };

    std::optional<PointT<float>> centroidOpt = centroidAlgorithm->calc(perfectStarImage, thresholdFunction);

    BOOST_CHECK(centroidOpt.has_value());
    BOOST_CHECK_CLOSE(centroidOpt.value().x(), 14.8288746, 0.001F);
    BOOST_CHECK_CLOSE(centroidOpt.value().y(), 13.9050961, 0.001F);
}

// TODO!!!
//TODO: Implement the other centroid algorithms... COG, WCOG, ?? SUB-PIXEL and MOMENT2?!
// TODO: Add further tests?
// TODO!!!



// TODO: Centroid calc depends on the actual algorithm. So the test cases below have to be refactored!
//       -> The old CentroidT class is about to be removed.
///**
///**
// * Check Centroid of empty image. Expectation is that valid()
// * returns false and the optional return value has no value.
// */
//    BOOST_AUTO_TEST_CASE(centroid_test_empty_image)
//    {
//        ImageT nullImage;
//
//        BOOST_CHECK_THROW(CentroidT::calculate(nullImage), CentroidExceptionT);
//    }

// * Test centroid calculation for a perfectly normal distributed star
// * with odd pixel count. The centroid is right in the middle of the image.
// */
//BOOST_AUTO_TEST_CASE(centroid_test_perfect_normal_distribution_centered_image)
//{
//    ImageT normalDistOddPixelCountImage("test_data/gaussian_normal_distribution_2d/gaussian_2d_sigma1_factor_65535_odd_101x101.tiff");
//
//    auto centroid = CentroidT::calculate(normalDistOddPixelCountImage);
//    BOOST_CHECK_CLOSE( centroid->x(), 50.0F, 0.001F );
//    BOOST_CHECK_CLOSE( centroid->y(), 50.0F, 0.001F );
//}
//
//
///**
// * Test centroid calculation for a perfectly normal distributed star
// * with odd pixel count. The centroid is at position (x=22,y=14).
// */
//BOOST_AUTO_TEST_CASE(centroid_test_perfect_normal_distribution_shifted_image)
//{
//    ImageT normalDistOddPixelCountImage("test_data/test_image_21.tif");
//
//    auto centroid = CentroidT::calculate(normalDistOddPixelCountImage);
//    BOOST_CHECK_CLOSE( centroid->x(), 22.0F, 0.1F );
//    BOOST_CHECK_CLOSE( centroid->y(), 14.0F, 0.1F );
//}


// TODO: Further tests...
// TODO: Extend centroid calc function to also allow specification of a "region of interest"
//        or a point around which the centroid should be determined?

BOOST_AUTO_TEST_SUITE_END();
