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

#include <memory>
#include <cmath>

#include "../../common/include/hfd.h"
#include "../../common/include/image.h"

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
 * TODO: Explain why the result is counter-intuitive i.e. why the resulting HFD
 *       is not the diameter of the inner circle which exactly cuts the area of
 *       the outer circle into half... Instead of the expected factor of 1/sqrt(2)
 *       the factor is 2/3. This can be easily explained...
 */
BOOST_AUTO_TEST_CASE(hfd_test_all_pixel_values_equal_1_test)
{
    ImageT plainImage("test_data/test_image_15.tif"); // All pixels have the value 1 - 120x120

    const float outerDiameter = 99;
    const float expectedHfd = (2.0F / 3.0F) * outerDiameter;

    BOOST_CHECK_CLOSE(HfdT::calculate(plainImage,
                                      outerDiameter,
                                      1.0F /* scale factor */,
                                      nullptr,
                                      nullptr /*no background threshold subtraction*/),
                      expectedHfd,
                      0.1F);

    BOOST_CHECK_CLOSE(HfdT::calculate(plainImage,
                                      outerDiameter,
                                      10.0F /* scale factor */,
                                      nullptr,
                                      nullptr /*no background threshold subtraction*/),
                      expectedHfd,
                      0.01F);

    BOOST_CHECK_CLOSE(HfdT::calculate(plainImage,
                                      outerDiameter,
                                      100.0F /* scale factor */,
                                      nullptr,
                                      nullptr /*no background threshold subtraction*/),
                      expectedHfd,
                      0.0001F);
}


/**
 * An artificial image where all the pixels have the same value > 0 is used to
 * check if the calculated HFD fits the expected theoretical value.
 */
BOOST_AUTO_TEST_CASE(hfd_test_all_pixel_values_equal_65535_test)
{
    ImageT whiteImage("test_data/test_image_14.tif"); // All pixels have the value 65535 - 120x120

    const float outerDiameter = 99;
    const float expectedHfd = (2.0F / 3.0F) * outerDiameter;

    BOOST_CHECK_CLOSE(HfdT::calculate(whiteImage,
                                      outerDiameter,
                                      1.0F /* scale factor */,
                                      nullptr,
                                      nullptr /*no background threshold subtraction*/),
                      expectedHfd,
                      0.1F);

    BOOST_CHECK_CLOSE(HfdT::calculate(whiteImage,
                                      outerDiameter,
                                      10.0F /* scale factor */,
                                      nullptr,
                                      nullptr /*no background threshold subtraction*/),
                      expectedHfd,
                      0.01F);

    BOOST_CHECK_CLOSE(HfdT::calculate(whiteImage,
                                      outerDiameter,
                                      100.0F /* scale factor */,
                                      nullptr,
                                      nullptr /*no background threshold subtraction*/),
                      expectedHfd,
                      0.0001F);
}


/**
 * TODO: Comment the formula...
 *
 * @param sigma
 * @return
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
BOOST_AUTO_TEST_CASE(hfd_test_ideal_gaussian_sigmaX_test)
{
    std::array<std::pair<float, float>, 5> sigmasAndMaxErrors = {
            std::make_pair(1.0F, 4.0F), std::make_pair(2.0F, 2.5F),
            std::make_pair(3.0F, 1.5F), std::make_pair(4.0F, 0.5F),
            std::make_pair(5.0F, 0.5F)
    };
    std::array<int, 2> imageDimensions = { 51, 101 };
    std::array<std::string, 3> normFactors = { "normalized", "factor_32767", "factor_65535" };


    for (auto imageDimension : imageDimensions) {
        for (const auto & normFactorStr : normFactors) {
            for (auto sigmaAndMaxError : sigmasAndMaxErrors) {

                float sigma = sigmaAndMaxError.first;
                float maxError = sigmaAndMaxError.second;

                std::stringstream filenameSs;
                filenameSs << "test_data/gaussian_normal_distribution_2d/gaussian_2d_sigma"
                           << (int) sigma << "_" << normFactorStr << "_odd_"
                           << imageDimension << "x" << imageDimension << ".tiff";

                ImageT img(filenameSs.str().c_str());

                std::cerr << "img: " << filenameSs.str() <<  ", dim - w=" << img.width() << ", h=" << img.height() << ", max value=" << img.max() << std::endl;

                BOOST_CHECK_CLOSE(HfdT::calculate(
                        img,
                        (double) imageDimension,
                        1.0F /* scale factor */,
                        nullptr,
                        nullptr /*no background threshold subtraction*/),
                        calcExpectedHfd(sigma), maxError);
            }
        }
    }
}


/**
 * Specify invalid "star center" and/or outer diameters.
 */
BOOST_AUTO_TEST_CASE(hfd_out_of_bounds_test) {
        ImageT image120x120("test_data/test_image_14.tif"); // 120x120

        // star center position completely out of range of the image dimensions
        // -> Expect exception
        // lower right corner
        BOOST_CHECK_THROW(HfdT::calculate(
                image120x120  /* pixel index 0..119 */,
                PointT<unsigned int>(120,120),
                3,
                1.0F /* scale factor */,
                nullptr,
                nullptr /*no background threshold subtraction*/),
                HfdExceptionT);

        // x coordinate out of bounds
        BOOST_CHECK_THROW(HfdT::calculate(
                image120x120  /* pixel index 0..119 */,
                PointT<unsigned int>(130,50),
                3,
                1.0F /* scale factor */,
                nullptr,
                nullptr /*no background threshold subtraction*/),
                HfdExceptionT);


        // star center position inside image dimensions but "outer diameter" outside image boundaries.
        // -> Expect exception
        // top left corner
        BOOST_CHECK_THROW(HfdT::calculate(
                image120x120 /* pixel index 0..119 */,
                PointT<unsigned int>(0,0),
                3,
                1.0F /* scale factor */,
                nullptr,
                nullptr /*no background threshold subtraction*/),
                HfdExceptionT);

        // lower right corner
        BOOST_CHECK_THROW(HfdT::calculate(
                image120x120 /* pixel index 0..119 */,
                PointT<unsigned int>(119,119),
                3,
                1.0F /* scale factor */,
                nullptr,
                nullptr /*no background threshold subtraction*/),
                HfdExceptionT);

        // star center is valid but outer radius completely exceeds image bounds
        BOOST_CHECK_THROW(HfdT::calculate(
                image120x120 /* pixel index 0..119 */,
                PointT<unsigned int>(60,60),
                151,
                1.0F /* scale factor */,
                nullptr,
                nullptr /*no background threshold subtraction*/),
                HfdExceptionT);
}


/**
 * Specify invalid "star center" and/or outer diameters.
 */
BOOST_AUTO_TEST_CASE(hfd_out_star_center_and_outer_diameter_corner_cases_test) {
        ImageT image120x120("test_data/test_image_14.tif"); // 120x120

        // Star center position and outer diameter at the border but still valid.
        // top left corner
        BOOST_CHECK_NO_THROW(HfdT::calculate(
                image120x120 /* pixel index 0..119 */,
                PointT<unsigned int>(1,1),
                3,
                1.0F /* scale factor */,
                nullptr,
                nullptr /*no background threshold subtraction*/));

        // left border
        BOOST_CHECK_NO_THROW(HfdT::calculate(
                image120x120 /* pixel index 0..119 */,
                PointT<unsigned int>(1,50),
                3,
                1.0F /* scale factor */,
                nullptr,
                nullptr /*no background threshold subtraction*/));

        // right border
        BOOST_CHECK_NO_THROW(HfdT::calculate(
                image120x120 /* pixel index 0..119 */,
                PointT<unsigned int>(118,50),
                3,
                1.0F /* scale factor */,
                nullptr,
                nullptr /*no background threshold subtraction*/));

        // bottom right corner
        BOOST_CHECK_NO_THROW(HfdT::calculate(
                image120x120 /* pixel index 0..119 */,
                PointT<unsigned int>(118,118),
                3,
                1.0F /* scale factor */,
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
