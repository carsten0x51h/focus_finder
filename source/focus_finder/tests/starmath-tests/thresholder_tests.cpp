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
#include <boost/test/tools/floating_point_comparison.hpp>

#include "../../common/include/thresholding_algorithm_factory.h"
#include "../../common/include/image.h"
#include "../../common/include/point.h"

BOOST_AUTO_TEST_SUITE(thresholder_tests)

/**
 * Test the mean thresholding algorithm using a plain, image with
 * all pixel values = 1. The calculated threshold is expected to
 * be 1, since this is the mean value of all pixels.
 *
 * TODO: Test if exception is thrown in certain cases?
 *
 * TODO: Which unit tests to define?? Black image? White image? Other artifical image?
 *       Real star image with noise? -> compare to "Moments" thresholder of ImageJ...
 */
BOOST_AUTO_TEST_CASE(mean_thresholding_plain_image_test)
{
    ImageT plainImage5x5("test_data/test_image_22.tif"); // 5x5 pixel, all pixel values = 1

    // TODO: Deal with depth...
    BOOST_CHECK_CLOSE(
            ThresholdingAlgorithmFactoryT::getInstance(ThresholdingAlgorithmTypeT::MEAN)->calc(plainImage5x5, 16),
            1.0F    /*expected threshold*/,
            0.001F  /*fault tolerance*/
    );
}

BOOST_AUTO_TEST_SUITE_END();
