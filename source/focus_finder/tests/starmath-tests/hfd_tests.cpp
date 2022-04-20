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
#include <cmath>

#include "../../common/include/hfd.h"
#include "../../common/include/image.h"

// TODO: Remove?
#define cimg_use_tiff
//#define cimg_use_png

BOOST_AUTO_TEST_SUITE(hfd_tests)

/**
 * Check HFD of empty image.
 * Expect a HfdExceptionT.
 */
BOOST_AUTO_TEST_CASE(hfd_test_empty_image)
{
    ImageT nullImage;
    BOOST_CHECK_THROW(HfdT::calculate(nullImage), HfdExceptionT);
}


/**
 * Check HFD of completely dark image
 *
 * What should be expected as an HFD value?
 *
 * One exception is the case when there is no flux at all (i.e. a totally black image).
 * In that case the HFD actually does not exist since there would be a division by 0.
 * Therefore, NaN is expected.
 *
 * Also see https://www.lost-infinity.com/night-sky-image-processing-part-6-measuring-the-half-flux-diameter-hfd-of-a-star-a-simple-c-implementation/
 */
BOOST_AUTO_TEST_CASE(hfd_dark_image_test)
{
    ImageT darkImage("test_data/test_image_2.tif");

    const float outerDiameter = 21;
    BOOST_CHECK_EQUAL(std::isnan(HfdT::calculate(darkImage, outerDiameter)), true);
}


/**
 * An artificial image where all the pixels have the same value > 0 is used to
 * check if the calculated HFD fits the expected theoretical value.
 *
 * TODO: Explain why the result is counter-intuitive i.e. why the resulting HFD
 *       is not the diameter of the inner circle which exactly cuts the area of
 *       the outer circle into half... Instead of the expected factor of 1/sqrt(2)
 *       the factor is 2/3. This can be easily explained...
 */
BOOST_AUTO_TEST_CASE(hfd_test_image1_test)
{
    ImageT whiteImage("test_data/test_image_14.tif"); // All pixels have the value 65534 - 120x120

    const float outerDiameter = 99;
    const float expectedHfd = (2.0F / 3.0F) * outerDiameter;

    BOOST_CHECK_CLOSE(HfdT::calculate(whiteImage, outerDiameter, 10.0F /* scale factor */, nullptr, false), expectedHfd, 0.001F);
}


// TODO: Further tests...

BOOST_AUTO_TEST_SUITE_END();
