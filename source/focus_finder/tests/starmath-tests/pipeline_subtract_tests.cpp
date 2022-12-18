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

#include <range/v3/range/conversion.hpp>

#include <boost/test/unit_test.hpp>

#include "../../common/include/pipeline/adapter/images.h"
#include "../../common/include/pipeline/adapter/subtract.h"

BOOST_AUTO_TEST_SUITE(pipeline_subtract_tests)

using namespace AstroImagePipeline;
using namespace ranges;

/**
 * Test if subtraction of an image from another image results in the
 * expected pixel values. All pixel values are expected to be positive.
 */
BOOST_AUTO_TEST_CASE(pipeline_subtract_image_positive_result_test)
{
    ImageT expectedResultImage(5,6,1,1,0); // 5x6 - bg value 0
    expectedResultImage(2,3) = 33000.0F;

    auto imageToSubtractPtr = std::make_shared<ImageT>("test_data/image_processing_pipeline/subtract/test_image_center_mixed_5x6.tiff");

    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/subtract/test_image_center_65000_5x6.tiff",
    };

    auto resultImagePtr = imageFilenames
                          | images()
                          | subtract(imageToSubtractPtr)
                          | to<std::vector>();

    // NOTE: Exactly one image is expected
    const ImageT & resultImage = *(resultImagePtr.at(0));

    BOOST_TEST(resultImage == expectedResultImage);
}


/**
 * Test if subtraction of an image from another image results in the
 * expected pixel values. Some pixel values are expected to be negative.
 */
BOOST_AUTO_TEST_CASE(pipeline_subtract_image_negaive_result_test)
{
    ImageT expectedResultImage(5,6,1,1,0); // 5x6 - bg value 0
    expectedResultImage(2,2) = -65000.0F;
    expectedResultImage(2,3) = -32000.0F;

    auto imageToSubtractPtr = std::make_shared<ImageT>("test_data/image_processing_pipeline/subtract/test_image_center_mixed_5x6.tiff");

    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/subtract/test_image_all_values_0_5x6.tiff",
    };

    auto resultImagePtr = imageFilenames
                          | images()
                          | subtract(imageToSubtractPtr)
                          | to<std::vector>();

    // NOTE: Exactly one image is expected
    const ImageT & resultImage = *(resultImagePtr.at(0));

    BOOST_TEST(resultImage == expectedResultImage);
}


/**
 * Test if subtraction of a scalar from another image results in the
 * expected pixel values. Some pixel values are expected to be negative.
 */
BOOST_AUTO_TEST_CASE(pipeline_subtract_scalar_test)
{
    ImageT expectedResultImage(5,6,1,1,-10); // 5x6 - bg value -10

    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/subtract/test_image_all_values_0_5x6.tiff",
    };

    auto resultImagePtr = imageFilenames
                          | images()
                          | subtract(10.0F)
                          | to<std::vector>();

    // NOTE: Exactly one image is expected
    const ImageT & resultImage = *(resultImagePtr.at(0));

    BOOST_TEST(resultImage == expectedResultImage);
}

BOOST_AUTO_TEST_SUITE_END();
