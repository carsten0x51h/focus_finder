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

#include "../../common/include/image.h"

/**
 * This image is expected to have 16 bit, each pixel is supposed to have
 * the pixel value 65535.
 */
BOOST_AUTO_TEST_CASE(tiff_16_bit_image_test)
{
        const float expectedPixelValue = 65535.0F;
        ImageT tiffFloatImage("test_data/test_image_7.tif");
        BOOST_CHECK_CLOSE(tiffFloatImage(0,0), expectedPixelValue, 0.001F);
}

/**
 * This image is expected to have 32 bit, each pixel is supposed to have
 * the pixel value 0.5.
 */
BOOST_AUTO_TEST_CASE(tiff_32_bit_float_image_test)
{
    const float expectedPixelValue = 0.5F;
    ImageT tiffFloatImage("test_data/test_image_20.tif");
    BOOST_CHECK_CLOSE(tiffFloatImage(0,0), expectedPixelValue, 0.001F);
}
