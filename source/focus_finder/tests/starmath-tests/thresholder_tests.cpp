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

// TODO: Create unit tests for Otsu, Max entropy, mean and second_moment thresholders ...


#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>

namespace bdata = boost::unit_test::data;

#include "../../common/include/thresholding_algorithm_factory.h"
#include "../../common/include/point.h"

BOOST_AUTO_TEST_SUITE(thresholder_tests)

/**
 * TODO: Which unit tests to define?? Black image? White image? Other artificial image?
 *       Real star image with noise? -> compare to "Moments" thresholder of ImageJ...
 */


/**
 * Thresholding empty image test. In case of an empty image, a
 * ThresholdingExceptionT is expected to be thrown.
 */
BOOST_AUTO_TEST_CASE(thresholding_empty_image_test)
{
    ImageT nullImage;
    BOOST_CHECK_THROW(ThresholdingAlgorithmFactoryT::getInstance(ThresholdingAlgorithmTypeT::MEAN)->calc(nullImage, 16), ThresholdingExceptionT);
    BOOST_CHECK_THROW(ThresholdingAlgorithmFactoryT::getInstance(ThresholdingAlgorithmTypeT::MAX_ENTROPY)->calc(nullImage, 16), ThresholdingExceptionT);
    BOOST_CHECK_THROW(ThresholdingAlgorithmFactoryT::getInstance(ThresholdingAlgorithmTypeT::OTSU)->calc(nullImage, 16), ThresholdingExceptionT);
}


/**
 * Test the mean thresholding algorithm using a plain, image with
 * all pixel values = 1. The calculated threshold is expected to
 * be 1, since this is the mean value of all pixels.
 *
 * TODO: Add further comments, extend test to other Thresholder types.
 * TODO: Deal with image bit depth...
 */
BOOST_DATA_TEST_CASE(thresholding_image_test,
     bdata::make(
             std::vector <std::string> {
                 "test_data/thresholding/test_image_all_pixels_1_5x5.tif",
                 "test_data/thresholding/test_image_thresholding_all_pixels_value_1_120x120.tif",
                 "test_data/thresholding/test_image_thresholding_all_pixels_value_65535_100x100.tif",
                 "test_data/thresholding/test_image_thresholding_6x6.tif"
             }) ^
     bdata::make(
             std::vector< std::tuple<ThresholdingAlgorithmTypeT::TypeE, float, float> > {
                 { ThresholdingAlgorithmTypeT::MEAN, 1.0F, 0.001F },
                 { ThresholdingAlgorithmTypeT::OTSU, 0.0F, 0.001F },
                 { ThresholdingAlgorithmTypeT::OTSU, 0.0F, 0.001F },
                 { ThresholdingAlgorithmTypeT::OTSU, 2.0F, 0.001F }
             }),
     imageFilename, thresholdingType, expectedThreshold, faultTolerance)
{
    ImageT image(imageFilename.c_str());

    BOOST_CHECK_CLOSE(
            ThresholdingAlgorithmFactoryT::getInstance(thresholdingType)->calc(image, 16),
            expectedThreshold,
            faultTolerance
    );
}

BOOST_AUTO_TEST_SUITE_END();
