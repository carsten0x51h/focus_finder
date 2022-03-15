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

#include "../../common/include/hfd.h"
#include "../../common/include/image.h"


BOOST_AUTO_TEST_SUITE(hfd_tests)

/**
 * Check HFD of empty image.
 * Expect a cimg_library::CImgInstanceException.
 */
BOOST_AUTO_TEST_CASE(hfd_test_empty_image)
{
    ImageT nullImage;
    BOOST_CHECK_THROW(HfdT::calculate(nullImage), HfdExceptionT);
}

// TODO: Further tests...

BOOST_AUTO_TEST_SUITE_END();
