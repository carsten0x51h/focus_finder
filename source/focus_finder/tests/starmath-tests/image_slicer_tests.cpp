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
#include <boost/test/test_tools.hpp>

#include "../../common/include/image.h"
#include "../../common/include/image_slicer.h"

struct ImageSlicerTestFixture {
    ImageSlicerTestFixture() {
        BOOST_TEST_MESSAGE( "Loading test image." );

        mTestImage = ImageT("test_data/test_image_4.tif");
    }
    ~ImageSlicerTestFixture() = default;

    ImageT mTestImage;
};

BOOST_FIXTURE_TEST_SUITE(image_slicer_tests, ImageSlicerTestFixture);


/**
 * Extract a horizontal and a vertical line from an image and compare it
 * against the expected values.
 */
BOOST_AUTO_TEST_CASE(slicing_test)
{
    // Horizontal slice check
    const size_t horzPos = 9;
    auto horzSlice = ImageSlicerT::slice<SliceDirectionT::HORZ>(mTestImage, horzPos);

    // Values manually crosschecked using ImageJ
    std::list<float> expectedHorzPixelsValues = {
            11667, 11554, 11694, 11630, 12303, 13797, 16955, 19371, 17333, 13835, 12139, 11776, 11751, 11373, 11769
    };

    BOOST_TEST(horzSlice == expectedHorzPixelsValues,
               boost::test_tools::tolerance(1e-3) << "all expected horizontal pixel values in slice"
                                                  << boost::test_tools::per_element());

    // Vertical slice check
    const size_t vertPos = 7;
    auto vertSlice = ImageSlicerT::slice<SliceDirectionT::VERT>(mTestImage, vertPos);

    // Values manually crosschecked using ImageJ
    std::list<float> expectedVertPixelsValues = {
            11423, 11740, 11665, 12039, 14459, 23210, 42693, 51836, 35324, 19371, 13353, 12142, 11577, 11421, 11717
    };

    BOOST_TEST(vertSlice == expectedVertPixelsValues,
               boost::test_tools::tolerance(1e-3) << "all expected vertical pixel values in slice"
                                                  << boost::test_tools::per_element());
}

/**
 * Pass in an invalid slicing direction and check if an
 * exception is thrown.
 */
BOOST_AUTO_TEST_CASE(slicing_invalid_direction_test)
{
    const size_t pos = 7;
    BOOST_CHECK_THROW( ImageSlicerT::slice<SliceDirectionT::_Count>(mTestImage, pos) , ImageSlicerExceptionT );
}

/**
 * Pass in a pixel position which exceeds the image size and
 * expect that an exception is thrown.
 */
BOOST_AUTO_TEST_CASE(slicing_out_of_bounds_position_test)
{
    const size_t invalidPos = 77;
    BOOST_CHECK_THROW( ImageSlicerT::slice<SliceDirectionT::HORZ>(mTestImage, invalidPos) , ImageSlicerExceptionT );
}

/**
 * Pass an empty image and expect an empty vector to be returned.
 */
BOOST_AUTO_TEST_CASE(slicing_empty_image_test)
{
    ImageT nullImage;
    auto sliceValues = ImageSlicerT::slice<SliceDirectionT::HORZ>(nullImage, 5);
    BOOST_TEST(sliceValues.size() == 0);
}

BOOST_AUTO_TEST_SUITE_END();
