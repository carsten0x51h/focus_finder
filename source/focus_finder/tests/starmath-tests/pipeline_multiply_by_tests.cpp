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

#include "../../common/include/pipeline/view/multiply_by.h"
#include "../../common/include/pipeline/view/read.h"

BOOST_AUTO_TEST_SUITE(pipeline_multiply_by_tests)

using namespace starmath;
using namespace starmath::pipeline;
using namespace starmath::pipeline::io;
using namespace ranges;

/**
 *
 */
BOOST_AUTO_TEST_CASE(pipeline_multiply_by_image_test)
{
    ImageT expectedResultImage(5,5,1,1,3); // 5x5 - bg value 3
    expectedResultImage(2,2) = 200.0F;

    auto imageToMultiplyByPtr = std::make_shared<ImageT>("test_data/image_processing_pipeline/multiply_by/test_image_all_1_center_2_5x5.tiff");

    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/multiply_by/test_image_all_3_center_100_5x5.tiff",
    };

    auto resultImagesPtr = imageFilenames
                           | read()
                           | multiply_by(imageToMultiplyByPtr)
                           | to<std::vector>();

    // NOTE: Exactly one image is expected
    const ImageT & resultImage = *(resultImagesPtr.at(0));

    BOOST_TEST(resultImage == expectedResultImage);
}

/**
 *
 */
BOOST_AUTO_TEST_CASE(pipeline_multiply_by_scalar_test)
{
    ImageT expectedResultImage(5,5,1,1,9); // 5x5 - bg value 9
    expectedResultImage(2,2) = 300.0F;

    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/multiply_by/test_image_all_3_center_100_5x5.tiff",
    };

    auto resultImagesPtr = imageFilenames
                           | read()
                           | multiply_by(3.0F)
                           | to<std::vector>();

    // NOTE: Exactly one image is expected
    const ImageT & resultImage = *(resultImagesPtr.at(0));

    BOOST_TEST(resultImage == expectedResultImage);
}

BOOST_AUTO_TEST_SUITE_END();
