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
#include "../../common/include/image.h"


struct SingleStarDetectorTestFixture {
    SingleStarDetectorTestFixture() {
        BOOST_TEST_MESSAGE( "Loading test image." );

        mStarWindowSize = SizeT<unsigned int>(35, 35);

        mTestImage = std::make_shared<ImageT>("test_data/test_image_1.tif");
    }
    ~SingleStarDetectorTestFixture() {}

    std::shared_ptr<ImageT> mTestImage;
    SizeT<unsigned int> mStarWindowSize;
    const float SNR_LIMIT = 1.0F;
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
    const PointT<float> poi(424.0F, 916.0F); // Position close to star - manually read with KStar image viewer

    SingleStarDetectorAlgorithmT singleStarDetectorAlgorithm(SNR_LIMIT, mStarWindowSize);
    auto result = singleStarDetectorAlgorithm.detect(mTestImage, poi);

	BOOST_CHECK_EQUAL( result.getStatus(), SingleStarDetectorAlgorithmT::ResultT::StatusT::SINGLE_STAR_DETECTED );
    BOOST_CHECK_CLOSE( result.getSnrLimit(), SNR_LIMIT, 0.001F );
    BOOST_CHECK_EQUAL( result.getNumStarsDetected(), 1 );
}


/**
 * Select an area too close to the image border.
 */
BOOST_AUTO_TEST_CASE(single_star_detector_test_out_of_bounds)
{
    const PointT<float> poi(5.0F, 100.0F); // Position close to left border
	
    SingleStarDetectorAlgorithmT singleStarDetectorAlgorithm(SNR_LIMIT, mStarWindowSize);
    auto result = singleStarDetectorAlgorithm.detect(mTestImage, poi);

    BOOST_CHECK_EQUAL( result.getStatus(), SingleStarDetectorAlgorithmT::ResultT::StatusT::STAR_WINDOW_OUT_OF_BOUNDS );
    BOOST_CHECK_CLOSE( result.getSnrLimit(), SNR_LIMIT, 0.001F );
    BOOST_CHECK_EQUAL( result.getNumStarsDetected(), 0 );
}

/**
 * Specify a null image.
 */
BOOST_AUTO_TEST_CASE(single_star_detector_test_null_image)
{
    const PointT<float> poi(100.0F, 100.0F); // Any valid position to satisfy compiler

    SingleStarDetectorAlgorithmT singleStarDetectorAlgorithm(SNR_LIMIT, mStarWindowSize);
    auto result = singleStarDetectorAlgorithm.detect(nullptr, poi);

    BOOST_CHECK_EQUAL( result.getStatus(), SingleStarDetectorAlgorithmT::ResultT::StatusT::NO_INPUT_IMAGE_SET );
    BOOST_CHECK_CLOSE( result.getSnrLimit(), SNR_LIMIT, 0.001F );
    BOOST_CHECK_EQUAL( result.getNumStarsDetected(), 0 );
}

/**
 * Bad SNR.
 */
BOOST_AUTO_TEST_CASE(single_star_detector_test_bad_snr)
{
    const PointT<float> poi(100.0F, 100.0F); // Any valid position without a star

    SingleStarDetectorAlgorithmT singleStarDetectorAlgorithm(SNR_LIMIT, mStarWindowSize);
    auto result = singleStarDetectorAlgorithm.detect(mTestImage, poi);

    BOOST_CHECK_EQUAL( result.getStatus(), SingleStarDetectorAlgorithmT::ResultT::StatusT::NO_STAR_FOUND_SNR_TOO_LOW );
    BOOST_CHECK_CLOSE( result.getSnrLimit(), SNR_LIMIT, 0.001F );
    BOOST_CHECK_EQUAL( result.getNumStarsDetected(), 0 );
}

/**
 * Multiple stars detected.
 */
BOOST_AUTO_TEST_CASE(single_star_detector_test_multi_star)
{
    const PointT<float> poi(453.0F, 951.0F); // Location with two stars

    SingleStarDetectorAlgorithmT singleStarDetectorAlgorithm(SNR_LIMIT, mStarWindowSize);
    auto result = singleStarDetectorAlgorithm.detect(mTestImage, poi);

    BOOST_CHECK_EQUAL( result.getStatus(), SingleStarDetectorAlgorithmT::ResultT::StatusT::UNEXPECTED_STAR_COUNT );
    BOOST_CHECK_CLOSE( result.getSnrLimit(), SNR_LIMIT, 0.001F );
    BOOST_CHECK_EQUAL( result.getNumStarsDetected(), 2 );
}

BOOST_AUTO_TEST_SUITE_END()
