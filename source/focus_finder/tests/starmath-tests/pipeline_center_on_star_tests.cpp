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

#include "../../common/include/pipeline/view/center_on_star.h"
#include "../../common/include/centroid_algorithm_factory.h"
#include "../../common/include/pipeline/view/read.h"

BOOST_AUTO_TEST_SUITE(pipeline_center_on_star_tests)

using namespace starmath;
using namespace starmath::pipeline;
using namespace starmath::pipeline::io;
using namespace ranges;

/**
 * This test applies the center_on_star() pipeline functionality on a test image
 * with one single star in the upper left corner. The expected result is an new
 * image where the center of the star is in the center of the image. Furthermore,
 * the expected image size is exactly the same. The "newly" created pixels are
 * expected to have the value 0 (e.g. top, left corner).
 */
BOOST_AUTO_TEST_CASE(pipeline_center_on_star_test)
{
    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/center_on_star/test_image_ideal_star_73x65.tiff"
    };

    auto resultImagePtr = imageFilenames
                          | read()
                          | center_on_star(CentroidAlgorithmFactoryT::getInstance(CentroidAlgorithmTypeT::IWC))
                          | to<std::vector>();

    // NOTE: Exactly one image is expected
    const ImageT & resultImage = *(resultImagePtr.at(0));

    BOOST_TEST(resultImage.width() == 73);
    BOOST_TEST(resultImage.height() == 65);
    BOOST_TEST(resultImage(36, 32) == 65535);
    BOOST_TEST(resultImage(0, 0) == 0);
}


BOOST_AUTO_TEST_SUITE_END();
