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

#include "../../common/include/single_star_detector_algorithm.h"
#include "../../common/include/cimg_fits_io.h"
#include "../../common/include/image.h"


struct SingleStarDetectorTestFixture {
    SingleStarDetectorTestFixture() {
        BOOST_TEST_MESSAGE( "Loading test image." );

        mTestImage = std::make_shared<ImageT>();
        long outBitPix = 0L;
        std::stringstream logStream;
        CImgFitsIOHelperT::readFits(mTestImage.get(), "test_data/test_image_1.fits", & outBitPix,  & logStream);
    }
    ~SingleStarDetectorTestFixture() {}

    std::shared_ptr<ImageT> mTestImage;
};

BOOST_FIXTURE_TEST_SUITE(starmath_tests, SingleStarDetectorTestFixture);

/**
 * SingleStarDetectorTest - Success path
 *
 * Test if a single star is detected correctly for a POI which is
 * close to the given star.
 *
 * NOTE: SNR and star position (centroid result) are not tested here since those
 *       are tested in their own unit tests. Testing them here again would introduce
 *       a dependency - i.e. after modifying the calculation of the SNR or the CentroidT
 *       this testcase would fail.
 */
BOOST_AUTO_TEST_CASE(single_star_detector_test_success_path)
{
    const float SNR_LIMIT = 1.0F;
    SizeT<unsigned int> starWindowSize(35, 35);
    const PointT<float> poi(424.0F, 916.0F); // Position close to star - manually read with KStar image viewer

    SingleStarDetectorAlgorithmT singleStarDetectorAlgorithm(SNR_LIMIT, starWindowSize);
    auto result = singleStarDetectorAlgorithm.detect(mTestImage, poi);

	BOOST_CHECK_EQUAL( result.getStatus(), SingleStarDetectorAlgorithmT::ResultT::StatusT::SINGLE_STAR_DETECTED );
    BOOST_CHECK_EQUAL( result.getSnrLimit(), SNR_LIMIT );
    BOOST_CHECK_EQUAL( result.getNumStarsDetected(), 1 );
}


/**
 * TODO
 */
BOOST_AUTO_TEST_CASE(single_star_detector_test_out_of_bounds)
{
    BOOST_CHECK_EQUAL( 2, 2 );
}

BOOST_AUTO_TEST_SUITE_END()
