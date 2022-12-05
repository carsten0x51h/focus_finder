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

/**
 * TODO: Rename image files - Encode dimensions - e.g. 120x120 and type
 *       of image (e.g. plain/all_values_1/gaussian). Adapt names in HFD
 *       tests as well. Why? test_image_15.tif does not say anything.
 */
BOOST_AUTO_TEST_SUITE(centroid_tests)


/**
 * Check Centroid of empty image. Expectation is that valid()
 * returns false and the optional return value has no value.
 */
BOOST_AUTO_TEST_CASE(centroid_test_empty_image)
{
    ImageT nullImage;
    auto centroidAlgorithmCog = CentroidAlgorithmFactoryT::getInstance(CentroidAlgorithmTypeT::COG);
    auto centroidAlgorithmIwc = CentroidAlgorithmFactoryT::getInstance(CentroidAlgorithmTypeT::IWC);

    BOOST_CHECK_THROW(centroidAlgorithmCog->calc(nullImage), CentroidExceptionT);
    BOOST_CHECK_THROW(centroidAlgorithmIwc->calc(nullImage), CentroidExceptionT);
}


/**
 * Calculate centroids using the center of gravity (COG) method.
 * This array contains pairs of filename and expected centroid.
 * The expected values were cross-checked manually.
 */
BOOST_AUTO_TEST_CASE(center_of_gravity_centroid_test)
{
    std::array<std::pair<std::string, PointT<float> >, 5> centroidData = {
            std::make_pair("test_data/centroiding/test_image_all_pixels_value_1_5x5.tif", PointT<float>(2.0F,2.0F)),
            std::make_pair("test_data/centroiding/test_image_one_pixel_bottom_right_5x5.tif", PointT<float>(3.0F,3.0F)),
            std::make_pair("test_data/centroiding/test_image_one_pixel_top_left_10x10.tif", PointT<float>(1.0F,1.0F)),
            std::make_pair("test_data/centroiding/test_image_two_opposite_pixels_5x5.tif", PointT<float>(2.0F,2.0F)),
            std::make_pair("test_data/centroiding/test_image_noise_on_the_left_and_one_center_pixel_5x5.tif", PointT<float>(1.88378F,2.0F))
    };

    auto centroidAlgorithm = CentroidAlgorithmFactoryT::getInstance(CentroidAlgorithmTypeT::COG);

    for (auto const & centroidTestRecord : centroidData) {
        std::string imageFilename = centroidTestRecord.first;
        ImageT testImage(imageFilename.c_str());
        PointT<float> expectedCentroidPoint = centroidTestRecord.second;
        std::optional<PointT<float>> centroidOpt = centroidAlgorithm->calc(testImage);

        BOOST_CHECK(centroidOpt.has_value());
        BOOST_CHECK_CLOSE(centroidOpt.value().x(), expectedCentroidPoint.x(), 0.001F);
        BOOST_CHECK_CLOSE(centroidOpt.value().y(), expectedCentroidPoint.y(), 0.001F);
    }
}


/**
 * Calculate centroids using the intensity weighted center (IWC)
 * method. This array contains pairs of filename and expected centroid.
 * The expected values were cross-checked manually.
 *
 *
 * For a black square image (e.g. test_image_all_pixels_value_1_5x5.tif),
 * the IWC value for a plain image can be easily calculated:
 *
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
BOOST_AUTO_TEST_CASE(intensity_weighted_centroid_test)
{
    std::array<std::pair<std::string, PointT<float> >, 6> centroidData = {
            std::make_pair("test_data/centroiding/test_image_all_pixels_value_1_5x5.tif", PointT<float>(2.0F,2.0F)),
            std::make_pair("test_data/centroiding/test_image_one_pixel_bottom_right_5x5.tif", PointT<float>(3.0F,3.0F)),
            std::make_pair("test_data/centroiding/test_image_one_pixel_top_left_10x10.tif", PointT<float>(1.0F,1.0F)),
            std::make_pair("test_data/centroiding/test_image_two_opposite_pixels_5x5.tif", PointT<float>(2.0F,2.0F)),
            std::make_pair("test_data/centroiding/test_image_ideal_star_73x65.tif", PointT<float>(22.0F,14.0F)),
            std::make_pair("test_data/centroiding/test_image_noise_on_the_left_and_one_center_pixel_5x5.tif", PointT<float>(1.99803F,2.0F))
    };

    auto centroidAlgorithm = CentroidAlgorithmFactoryT::getInstance(CentroidAlgorithmTypeT::IWC);

    for (auto const & centroidTestRecord : centroidData) {
        std::string imageFilename = centroidTestRecord.first;
        ImageT testImage(imageFilename.c_str());
        PointT<float> expectedCentroidPoint = centroidTestRecord.second;
        std::optional<PointT<float>> centroidOpt = centroidAlgorithm->calc(testImage);

        BOOST_CHECK(centroidOpt.has_value());
        BOOST_CHECK_CLOSE(centroidOpt.value().x(), expectedCentroidPoint.x(), 0.001F);
        BOOST_CHECK_CLOSE(centroidOpt.value().y(), expectedCentroidPoint.y(), 0.001F);
    }
}

// TODO: Implement the other centroid algorithms... WCOG, ?? SUB-PIXEL and MOMENT2?!
// TODO: Add further tests?
// TODO: Remove old Centroid from code

BOOST_AUTO_TEST_SUITE_END();
