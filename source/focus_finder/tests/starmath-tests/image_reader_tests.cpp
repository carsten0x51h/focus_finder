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
#include <boost/test/data/test_case.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>

#include "../../common/include/image_reader.h"

namespace bdata = boost::unit_test::data;

/**
 *
 */
BOOST_DATA_TEST_CASE(fits_image_reader_test,
        bdata::make(
            std::vector<std::string> {
                "test_data/image_reader/test_image_fits_496x380.fit",
                "test_data/image_reader/test_image_fits_496x380.FIT",
                "test_data/image_reader/test_image_fits_496x380.fits",
                "test_data/image_reader/test_image_fits_496x380.Fits",
                "test_data/image_reader/test_image_fits_496x380.FITS",
                "test_data/image_reader/test_image_fits_496x380.fit.gz",
                "test_data/image_reader/test_image_fits_496x380.FIT.GZ",
                "test_data/image_reader/test_image_fits_496x380.fits.gz",
                "test_data/image_reader/test_image_fits_496x380.fits.GZ",
                "test_data/image_reader/test_image_fits_496x380.Fits.GZ",
                "test_data/image_reader/test_image_fits_496x380.FITS.GZ",
                "test_data/image_reader/test_image_16bit_100x100.tif",
                "test_data/image_reader/test_image_32bit_120x120.tif"
            }) ^
        bdata::make(
            // img width, img height, pixel x pos, pixel y pos, exp. pixel value
            std::vector< std::tuple<int, int, int, int, float> > {
                { 496, 380, 431, 203, 65535.0F },
                { 496, 380, 431, 203, 65535.0F },
                { 496, 380, 431, 203, 65535.0F },
                { 496, 380, 431, 203, 65535.0F },
                { 496, 380, 431, 203, 65535.0F },
                { 496, 380, 431, 203, 65535.0F },
                { 496, 380, 431, 203, 65535.0F },
                { 496, 380, 431, 203, 65535.0F },
                { 496, 380, 431, 203, 65535.0F },
                { 496, 380, 431, 203, 65535.0F },
                { 496, 380, 431, 203, 65535.0F },
                { 100, 100,   0,   0, 65535.0F }, // Img should have 16 bit, each pixel should have pixel value 65535.
                { 120, 120,   0,   0,     0.5F }, // Img should have 32 bit, each pixel should have pixel value 0.5.
            }),
        image_filename, image_width, image_height, pos_x, pos_y, pixel_value)
{
    auto img_ptr = starmath::io::read(image_filename);

    DEBUG_IMAGE_DISPLAY(*img_ptr, "image_reader_test", 0);

    BOOST_TEST(img_ptr->width() == image_width);
    BOOST_TEST(img_ptr->height() == image_height);
    BOOST_CHECK_CLOSE((*img_ptr)(pos_x, pos_y), pixel_value, 0.001F);
}
