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

#include <memory>

#include <range/v3/range/conversion.hpp>

#include <boost/test/unit_test.hpp>

#include "../../common/include/pipeline/view/crop.h"
#include "../../common/include/pipeline/view/read.h"

BOOST_AUTO_TEST_SUITE(pipeline_crop_tests)

using namespace starmath;
using namespace starmath::pipeline;
using namespace starmath::pipeline::io;
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
                          | read()
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
                          | read()
                          | crop_from_center(SizeT<int>(5,5))
                          | to<std::vector>();

    // NOTE: Exactly one image is expected
    BOOST_TEST(*(resultImagePtr.at(0)) == expectedResultImage);
}


/**
 * range<image>   -->  crop(rects)  --> range < range <image> >
 *       ^                                          ^
 *       |                                          |
 *  input image                         range of cropped images
 *
 *  Per input image, N cropped images (one per rect).
 */
BOOST_AUTO_TEST_CASE(pipeline_multi_crop_on_image_test)
{
	std::vector<RectT<int>> rects = { RectT<int>(0,0,10,10), RectT<int>(11,11,10,10) };

	// TODO: Choose an input image from the crop test folder!
    auto croppedImages =
	  view::single("test_data/image_processing_pipeline/real_world/star_recognizer/test_image_star_recognizer_1.fit.gz")
	  | read()
	  | crop(rects)
	  | to<std::vector>();

    BOOST_TEST(croppedImages.size() == 1);       // One image goes in, one result is produced (which wraps a vector)
//    BOOST_TEST(croppedImages.at(0).size() == 2); // Two rects as input produce two output images
}

/** 
 * range<image>   -->  crop(rect)  --> range <image>
 *        ^                                     ^
 *        |                                     |
 *   input image                          cropped image
 *
 * Per input image, one cropped image.
 */
BOOST_AUTO_TEST_CASE(pipeline_crop_on_image_test)
{
	RectT<int> rect1 = RectT<int>(0,0,10,10);
	
	// TODO: Choose an input image from the crop test folder! Choose 2 images ... at least...
    auto croppedImages =
	  view::single("test_data/image_processing_pipeline/real_world/star_recognizer/test_image_star_recognizer_1.fit.gz")
	  | read()
	  | crop(rect1)
	  | to<std::vector>();
	
	std::cerr << "N crops from N images... N=" << croppedImages.size() << std::endl;

	BOOST_TEST(croppedImages.size() == 1); // One image goes in, one result is produced
}


// TODO: Add crop_from_center test for even sized regions.
// TODO: Test specified region exceeding the image dimensions.

// TODO: Add "crop()" test

BOOST_AUTO_TEST_SUITE_END();
