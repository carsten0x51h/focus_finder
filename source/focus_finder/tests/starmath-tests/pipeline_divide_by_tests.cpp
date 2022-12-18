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

#include "../../common/include/pipeline/view/images.h"
#include "../../common/include/pipeline/view/divide_by.h"

BOOST_AUTO_TEST_SUITE(pipeline_divide_by_tests)

using namespace AstroImagePipeline;
using namespace ranges;

/**
 * An 5x5 image with all pixel values set to 1000 is divided by
 * another image where all pixel values are set to 1 - only the
 * one in the center is set to 2. The expectation is that this
 * pixel will have the value 500 while all other pixels will still
 * have the pixel value 1000.
 */
BOOST_AUTO_TEST_CASE(pipeline_divide_by_image_test)
{
    ImageT expectedResultImage(5,5,1,1,1000); // 5x5 - bg value 1000
    expectedResultImage(2,2) = 500.0F;

    auto divisorImagePtr = std::make_shared<ImageT>("test_data/image_processing_pipeline/divide/test_image_divisor_5x5.tiff");

    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/divide/test_image_all_pixels_1000_5x5.tiff",
    };

    auto resultImagePtr = imageFilenames
                          | images()
                          | divide_by(divisorImagePtr)
                          | to<std::vector>();

    // NOTE: Exactly one image is expected
    const ImageT & resultImage = *(resultImagePtr.at(0));

    BOOST_TEST(resultImage == expectedResultImage);
}

// TODO: Add divide_by(scalar) !

BOOST_AUTO_TEST_SUITE_END();
