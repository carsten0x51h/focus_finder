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

#include "../../common/include/pipeline/view/read.h"
#include "../../common/include/pipeline/view/scale.h"

BOOST_AUTO_TEST_SUITE(pipeline_scale_tests)

using namespace starmath;
using namespace starmath::pipeline;
using namespace starmath::pipeline::io;
using namespace ranges;

/**
 *
 */
BOOST_AUTO_TEST_CASE(pipeline_scale_up_test)
{
    ImageT expectedResultImage(10,10,1,1,250); // 10x10 - bg value 250
    expectedResultImage(4,4) = 65535.0F;   // Bright pixel at the center
    expectedResultImage(4,5) = 65535.0F;   // Bright pixel at the center
    expectedResultImage(5,4) = 65535.0F;   // Bright pixel at the center
    expectedResultImage(5,5) = 65535.0F;   // Bright pixel at the center

    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/scale/test_image_scale_5x5.tiff",
    };

    auto resultImagePtr = imageFilenames
                          | read()
                          | scale_up(2.0F)
                          | to<std::vector>();

    // NOTE: Exactly one image is expected
    const ImageT & resultImage = *(resultImagePtr.at(0));

    BOOST_TEST(resultImage.width() == 10);
    BOOST_TEST(resultImage.height() == 10);
    BOOST_TEST(resultImage == expectedResultImage);
}

/**
 *
 */
BOOST_AUTO_TEST_CASE(pipeline_scale_down_test)
{
    ImageT expectedResultImage(5,5,1,1,250); // 5x5 - bg value 250
    expectedResultImage(2,2) = 65535.0F;     // Bright pixel at the center

    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/scale/test_image_scale_10x10.tiff",
    };

    auto resultImagePtr = imageFilenames
                          | read()
                          | scale_down(2.0F)
                          | to<std::vector>();

    // NOTE: Exactly one image is expected
    const ImageT & resultImage = *(resultImagePtr.at(0));

    BOOST_TEST(resultImage.width() == 5);
    BOOST_TEST(resultImage.height() == 5);
    BOOST_TEST(resultImage == expectedResultImage);
}


BOOST_AUTO_TEST_SUITE_END();
