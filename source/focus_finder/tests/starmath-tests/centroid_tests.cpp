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

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>

#include <memory>

#include "../../common/include/centroid.h"
#include "../../common/include/image.h"


BOOST_AUTO_TEST_SUITE(centroid_tests)

/**
 * Check Centroid of empty image. Expectation is that valid()
 * returns false and the optional return value has no value.
 */
BOOST_AUTO_TEST_CASE(centroid_test_empty_image)
{
    ImageT nullImage;

    BOOST_CHECK_THROW(CentroidT::calculate(nullImage), CentroidExceptionT);
}


/**
 * Test centroid calculation for a perfectly normal distributed star
 * with odd pixel count. The centroid is right in the middle of the image.
 */
BOOST_AUTO_TEST_CASE(centroid_test_perfect_normal_distribution_centered_image)
{
    ImageT normalDistOddPixelCountImage("test_data/gaussian_normal_distribution_2d/gaussian_2d_sigma1_factor_65535_odd_101x101.tiff");

    auto centroid = CentroidT::calculate(normalDistOddPixelCountImage);
    BOOST_CHECK_CLOSE( centroid->x(), 50.0F, 0.001F );
    BOOST_CHECK_CLOSE( centroid->y(), 50.0F, 0.001F );
}


/**
 * Test centroid calculation for a perfectly normal distributed star
 * with odd pixel count. The centroid is at position (x=22,y=14).
 */
BOOST_AUTO_TEST_CASE(centroid_test_perfect_normal_distribution_shifted_image)
{
    ImageT normalDistOddPixelCountImage("test_data/test_image_21.tif");

    auto centroid = CentroidT::calculate(normalDistOddPixelCountImage);
    BOOST_CHECK_CLOSE( centroid->x(), 22.0F, 0.1F );
    BOOST_CHECK_CLOSE( centroid->y(), 14.0F, 0.1F );
}


// TODO: Further tests...
// TODO: Extend centroid calc function to also allow specification of a "region of interest"
//        or a point around which the centroid should be determined?

BOOST_AUTO_TEST_SUITE_END();
