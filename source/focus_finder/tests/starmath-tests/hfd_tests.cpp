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

#include <memory>
#include <cmath>

#include "../../common/include/hfd.h"

namespace bdata = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(hfd_tests)

/**
 * Check HFD of empty image.
 * Expect a HfdExceptionT.
 */
BOOST_AUTO_TEST_CASE(hfd_test_empty_image)
{
    ImageT nullImage;
    BOOST_CHECK_THROW(HfdT::calculate(nullImage), HfdExceptionT);
}

// TODO: Move test images to new hfd folder!... and rename them!

/**
 * Check HFD of completely dark image
 *
 * What should be expected as an HFD value?
 *
 * One exception is the case when there is no flux at all (i.e. a totally black image).
 * In that case the HFD actually does not exist since there would be a division by 0.
 * Therefore, NaN is expected.
 *
 * Also see https://www.lost-infinity.com/night-sky-image-processing-part-6-measuring-the-half-flux-diameter-hfd-of-a-star-a-simple-c-implementation/
 */
BOOST_AUTO_TEST_CASE(hfd_dark_image_test)
{
    ImageT darkImage("test_data/test_image_2.tif");

    const float outerDiameter = 21;
    BOOST_CHECK_EQUAL(std::isnan(HfdT::calculate(darkImage, outerDiameter)), true);
}


/**
 * Same as hfd_test_all_pixel_values_equal_1_test, expectation is
 * that pixel value does not make a difference in calculated HFD value.
 *
 * A detailed explanation why the expected HFD equals (2.0F / 3.0F) * outerDiameter
 * can be found here:
 *
 * https://www.lost-infinity.com/the-half-flux-diameter-hfd-of-a-plain-image/
 */
BOOST_DATA_TEST_CASE(hfd_test_all_pixel_values_equal_1_test,
                     bdata::make(
                        std::vector< std::string > {
                            "test_data/hfd/test_image_all_pixels_1_120x120.tif",
                            "test_data/hfd/test_image_all_pixels_65535_120x120.tif"
                     }) *
                     bdata::make(
                        std::vector< std::tuple<float, float> > {
                            { 1.0F, 0.1F },
                            { 10.0F, 0.01F },
                            { 100.0F, 0.0001F }
                     }),
                     imageFilename, scaleFactor, acceptedError)
{
    ImageT image(imageFilename.c_str());

    const float outerDiameter = 99;
    const float expectedHfd = (2.0F / 3.0F) * outerDiameter;

    BOOST_CHECK_CLOSE(HfdT::calculate(image,
                                      outerDiameter,
                                      scaleFactor,
                                      nullptr,
                                      nullptr /*no background threshold subtraction*/),
                      expectedHfd,
                      acceptedError);
}


/**
 * A detailed explanation of the formula can be found here:
 *
 * https://www.lost-infinity.com/the-half-flux-diameter-hfd-for-a-perfectly-normal-distributed-star/
 *
 * @param sigma
 * @return The expected HFD
 */
static double calcExpectedHfd(double sigma) {
    // NOTE: Using C++20, those constants can be replaced by <numbers> include.
    //       See: https://en.cppreference.com/w/cpp/numeric/constants
    const double FACTOR = 2.0 * std::tgamma(1.5) * std::sqrt(2.0);
    return FACTOR * sigma;
}


/**
 * Calculation of the HFD for an ideal gaussian shaped distribution.
 */
BOOST_DATA_TEST_CASE(hfd_test_ideal_gaussian_sigmaX_test,
                     bdata::make(
                             std::vector< std::tuple<float, float> > {
                                     { 1.0F, 4.0F },
                                     { 2.0F, 2.5F },
                                     { 3.0F, 1.5F },
                                     { 4.0F, 0.5F },
                                     { 5.0F, 0.5F }
                             }) *
                     bdata::make(
                             std::vector< int > {
                                 51, 101
                             }) *
                     bdata::make(
                             std::vector< std::string > {
                                     "normalized",
                                     "factor_32767",
                                     "factor_65535"
                             }),
                     sigma, maxError, imageDimension, normFactorStr)
{
    std::stringstream filenameSs;
    filenameSs << "test_data/gaussian_normal_distribution_2d/gaussian_2d_sigma"
               << (int) sigma << "_" << normFactorStr << "_odd_"
               << imageDimension << "x" << imageDimension << ".tiff";

    ImageT img(filenameSs.str().c_str());

    BOOST_CHECK_CLOSE(HfdT::calculate(
            img,
            (double) imageDimension,
            1.0F /* scale factor */,
            nullptr,
            nullptr /*no background threshold subtraction*/),
            calcExpectedHfd(sigma), maxError);
}


/**
 * Specify invalid "star center" and/or outer diameters.
 */
BOOST_DATA_TEST_CASE(hfd_out_of_bounds_test,
                     (bdata::make(
                             std::vector< PointT<unsigned int> > {
                                     PointT<unsigned int>(120,120), // star center position completely out of range of the image dimensions, lower right corner
                                     PointT<unsigned int>(130,50),  // x coordinate out of bounds
                                     PointT<unsigned int>(0,0),     // star center position inside image dimensions but "outer diameter" outside image boundaries, top left corner
                                     PointT<unsigned int>(119,119), // lower right corner
                                     PointT<unsigned int>(60,60)
                             }) ^
                     bdata::make(
                             std::vector< int > {
                                     3, 3, 3, 3,
                                     151            // star center is valid but outer radius completely exceeds image bounds
                             })) *
                     bdata::make(
                             std::vector< float > {
                                     1.0F, 10.0F
                             }),
                     starCenter, outerDiameter, scaleFactor)
{
        ImageT image120x120("test_data/hfd/test_image_all_pixels_65535_120x120.tif"); // 120x120

        BOOST_CHECK_THROW(HfdT::calculate(
                image120x120  /* pixel index 0..119 */,
                starCenter,
                outerDiameter,
                scaleFactor,
                nullptr,
                nullptr /*no background threshold subtraction*/),
                HfdExceptionT);
}


/**
 * Specify invalid "star center" and/or outer diameters.
 */
BOOST_DATA_TEST_CASE(hfd_out_star_center_and_outer_diameter_corner_cases_test,
                     bdata::make(
                             std::vector< PointT<unsigned int> > {
                                     PointT<unsigned int>(1,1),     // Star center position and outer diameter at the border but still valid - top left corner
                                     PointT<unsigned int>(1,50),    // left border
                                     PointT<unsigned int>(118,50),  // right border
                                     PointT<unsigned int>(118,118)  // bottom right corner
                             }) *
                     bdata::make(
                             std::vector< float > {
                                     1.0F, 10.0F
                             }),
                     starCenter, scaleFactor)
{
        ImageT image120x120("test_data/hfd/test_image_all_pixels_65535_120x120.tif"); // 120x120

        BOOST_CHECK_NO_THROW(HfdT::calculate(
                image120x120 /* pixel index 0..119 */,
                starCenter,
                3,
                scaleFactor,
                nullptr,
                nullptr /*no background threshold subtraction*/));
}


/**
 * The HFD algorithm is executed on a set of real star images recorded with a Newtonian
 * telescope. The telescope focus is moved from "far out of focus" to "in focus". In sum
 * 11 images were recorded. The focus distance is equal between the images. The star
 * image never reaches saturation. The star center slightly moves what is ignored in this
 * test.
 */
BOOST_AUTO_TEST_CASE(hfd_real_newton_focus_star_no_background_subtraction_test)
{
    std::array<std::pair<int, double>, 11> focusStarData = {
            std::make_pair(1, 27.456088096660295), std::make_pair(2, 26.039354380832002),
            std::make_pair(3, 24.798843695746001), std::make_pair(4, 22.743999207494756),
            std::make_pair(5, 19.926210044511915), std::make_pair(6, 17.341647262965008),
            std::make_pair(7, 14.616004395669748), std::make_pair(8, 12.516580080411764),
            std::make_pair(9, 10.090836864932291), std::make_pair(10, 7.0064759328706696),
            std::make_pair(11, 6.3337311227004358)
    };

    for (auto focusStar : focusStarData) {
        int focusStarNumber = focusStar.first;
        double expectedFocusStarHfd = focusStar.second;

        std::stringstream filenameSs;
        filenameSs << "test_data/newton_focus_star/newton_focus_star" << focusStarNumber << ".tiff";

        // For the unit-tests a very simple background threshold function is used - the mean() function.
        // NOTE: Instead the "Max entropy" or "Otsu" thresholding algorithm can be used.
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

}


// TODO: Add real star images (see recorded images in different focus) also containing noise...
// TODO: Further tests...

BOOST_AUTO_TEST_SUITE_END();
