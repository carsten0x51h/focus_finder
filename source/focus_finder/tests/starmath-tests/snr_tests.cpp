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

namespace bdata = boost::unit_test::data;

#include <memory>

#include "../../common/include/snr.h"


BOOST_AUTO_TEST_SUITE(snr_tests)

/**
 * Check SNR of empty image. A cimg_library::CImgInstanceException is expected.
 */
BOOST_AUTO_TEST_CASE(snr_test_empty_image)
{
    ImageT nullImage;

    BOOST_CHECK_THROW( SnrT::calculate(nullImage), cimg_library::CImgInstanceException );
}

/**
 * Calculate SNR of different images.
 */
BOOST_DATA_TEST_CASE(snr_star_signals_test,
        bdata::make(
        std::vector< std::tuple<std::string, float> > {
            { "test_data/snr/test_image_all_pixels_0_100x100.tif", 0.0F }, // A completely black image should give an SNR of 0.
            { "test_data/snr/test_image_weak_star_30x31.tif", 1.5995403692606724F }, // A weak star
            { "test_data/snr/test_image_bright_star_15x15.tif", 4.1645735440908789F }, // A bright star
            { "test_data/snr/test_image_saturated_star_15x15.tif", 4.2979500980918717F }, // A saturated star
            { "test_data/snr/test_image_noise_26x26.tif", 1.4864298210463467F }, // Noise
            { "test_data/snr/test_image_all_pixels_65535_100x100.tif", 0.0F } // A completely white image should give an SNR of 0.
        }),
        imageFilename, snr)
{
    ImageT img(imageFilename.c_str());
    BOOST_CHECK_CLOSE( SnrT::calculate(img), snr, 0.001F );
}

BOOST_AUTO_TEST_SUITE_END();
