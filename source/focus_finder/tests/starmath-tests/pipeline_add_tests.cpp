/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finer.
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

#include "../../common/include/pipeline/view/images.h"
#include "../../common/include/pipeline/view/add.h"

BOOST_AUTO_TEST_SUITE(pipeline_add_tests)

    using namespace AstroImagePipeline;
    using namespace ranges;

/**
 * Test if subtraction of an image from another image results in the
 * expected pixel values. All pixel values are expected to be positive.
 */
BOOST_AUTO_TEST_CASE(pipeline_add_image_test)
{
    ImageT expectedResultImage(5,5,1,1,4); // 5x5 - bg value 4
    expectedResultImage(2,2) = 102.0F;

    auto imageToAddPtr = std::make_shared<ImageT>("test_data/image_processing_pipeline/add/test_image_all_3_center_100_5x5.tiff");

    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/add/test_image_all_1_center_2_5x5.tiff",
    };

    auto resultImagePtr = imageFilenames
                          | images()
                          | add(imageToAddPtr)
                          | to<std::vector>();

    // NOTE: Exactly one image is expected
    const ImageT & resultImage = *(resultImagePtr.at(0));

    BOOST_TEST(resultImage == expectedResultImage);
}

/**
 * Test if subtraction of a scalar from an image works.
 */
BOOST_AUTO_TEST_CASE(pipeline_add_scalar_test)
{
    ImageT expectedResultImage(5,5,1,1,501); // 5x5 - bg value 501
    expectedResultImage(2,2) = 502.0F;

    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/add/test_image_all_1_center_2_5x5.tiff",
    };

    auto resultImagePtr = imageFilenames
                          | images()
                          | add(500.0F)
                          | to<std::vector>();

    // NOTE: Exactly one image is expected
    const ImageT & resultImage = *(resultImagePtr.at(0));

    BOOST_TEST(resultImage == expectedResultImage);
}

BOOST_AUTO_TEST_SUITE_END();
