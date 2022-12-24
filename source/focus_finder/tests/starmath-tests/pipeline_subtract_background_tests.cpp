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
#include "../../common/include/pipeline/view/subtract_background.h"
#include "../../common/include/thresholding_algorithm_factory.h"

BOOST_AUTO_TEST_SUITE(pipeline_subtract_background_tests)

using namespace starmath::pipeline;
using namespace starmath;
using namespace ranges;

/**
 * Test image 5x5 pixel, all background pixel values 250, center pixel 65535.
 * Thresholding algorithm mean. Given the pixel values, the mean threshold is 2861,4.
 * Subtracting the background should produces a 5x5 pixel picture, where all background
 * pixels should be 0 and the only center pixel should be 65535-2861,4.
 * This way it is also checked that negative results of the subtraction are value
 * clipped to 0.
 */
BOOST_AUTO_TEST_CASE(pipeline_subtract_background_1_test)
{
    ImageT expectedResultImage(5,5,1,1,0); // 5x5 - black
    expectedResultImage(2,2) = 62673.6F;   // Set the center pixel to the expected value

    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/subtract_background/test_image_subtract_background_5x5.tiff",
    };

    auto resultImagePtr = imageFilenames
            | images()
            | subtract_background(ThresholdingAlgorithmFactoryT::getInstance(ThresholdingAlgorithmTypeT::MEAN))
            | to<std::vector>();

    // NOTE: Exactly one image is expected
    BOOST_TEST(*(resultImagePtr.at(0)) == expectedResultImage);
}

BOOST_AUTO_TEST_SUITE_END();
