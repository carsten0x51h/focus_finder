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

#include "../../common/include/snr.h"
#include "../../common/include/image.h"

struct SnrTestFixture {
    SnrTestFixture() {
        BOOST_TEST_MESSAGE( "Loading test image." );
        mTestImage = ImageT("test_data/test_image_2.tif");
    }
    ~SnrTestFixture() {}

    ImageT mTestImage;
};

BOOST_FIXTURE_TEST_SUITE(starmath_tests, SnrTestFixture);

/**
 * A completely black image should give an SNR of 0.
 */
BOOST_AUTO_TEST_CASE(snr_test_null_signal)
{
	double snr = SnrT::calculate(mTestImage);
	
    BOOST_CHECK_CLOSE( snr, 0.0F, 0.001F );
}

BOOST_AUTO_TEST_SUITE_END()
