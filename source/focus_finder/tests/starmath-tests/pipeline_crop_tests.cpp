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
#include "../../common/include/pipeline/adapter/crop.h"

BOOST_AUTO_TEST_SUITE(pipeline_crop_tests)

using namespace AstroImagePipeline;
using namespace ranges;

/**
 * Test cropping a 3x3 rectangle from a 5x5 image with a
 * brihgt bixel in the center.
 */
BOOST_AUTO_TEST_CASE(pipeline_crop_from_center_sub_region_test)
{
    ImageT expectedResultImage(3,3,1,1,250); // 3x3 - bg value 250
    expectedResultImage(1,1) = 65535.0F;   // Bright pixel at the center

    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/crop/test_image_crop_from_center_5x5.tiff",
    };

    auto resultImagePtr = imageFilenames
                          | images()
                          | crop_from_center(SizeT<int>(3,3))
                          | to<std::vector>();

    // NOTE: Exactly one image is expected
    BOOST_TEST(*(resultImagePtr.at(0)) == expectedResultImage);
}


/**
 * Test cropping a 5x5 rectangle from a 5x5 image with a
 * bright pixel in the center.
 */
BOOST_AUTO_TEST_CASE(pipeline_crop_from_center_full_image_test)
{
    ImageT expectedResultImage(5,5,1,1,250); // 5x5 - bg value 250
    expectedResultImage(2,2) = 65535.0F;   // Bright pixel at the center

    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/crop/test_image_crop_from_center_5x5.tiff",
    };

    auto resultImagePtr = imageFilenames
                          | images()
                          | crop_from_center(SizeT<int>(5,5))
                          | to<std::vector>();

    // NOTE: Exactly one image is expected
    BOOST_TEST(*(resultImagePtr.at(0)) == expectedResultImage);
}

// TODO: Add crop_from_center test for even sized regions.
// TODO: Test specified region exceeding the image dimensions.

// TODO: Add "crop()" test

BOOST_AUTO_TEST_SUITE_END();
