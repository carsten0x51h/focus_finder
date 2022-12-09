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

#include "../../common/include/single_star_detector_algorithm.h"


struct SingleStarDetectorTestFixture {
    SingleStarDetectorTestFixture() {
        BOOST_TEST_MESSAGE( "Loading test image." );

        mStarWindowSize = SizeT<unsigned int>(35, 35);

        mTestImage = std::make_shared<ImageT>("test_data/test_image_1.tif");
    }
    ~SingleStarDetectorTestFixture() = default;

    std::shared_ptr<ImageT> mTestImage;
    SizeT<unsigned int> mStarWindowSize;
    const float SNR_LIMIT = 1.0F;
};

BOOST_FIXTURE_TEST_SUITE(single_star_detector_tests, SingleStarDetectorTestFixture);


BOOST_DATA_TEST_CASE(single_star_detector_test,
     bdata::make(
         std::vector< PointT<float> > {
                 PointT<float>(424.0F, 916.0F), // Position close to star - manually read with KStar image viewer
                 PointT<float>(5.0F, 100.0F),   // Position close to left border
                 PointT<float>(100.0F, 100.0F), // Any valid position to satisfy compiler
                 PointT<float>(452.0F, 943.0F)  // Location with two stars
         }) ^
     bdata::make(
         std::vector< std::tuple<int /*num stars detected*/, typename SingleStarDetectorAlgorithmT::ResultT::StatusT::TypeE> > {
                 { 1, SingleStarDetectorAlgorithmT::ResultT::StatusT::SINGLE_STAR_DETECTED },
                 { 0, SingleStarDetectorAlgorithmT::ResultT::StatusT::STAR_WINDOW_OUT_OF_BOUNDS },
                 { 0, SingleStarDetectorAlgorithmT::ResultT::StatusT::NO_STAR_FOUND_SNR_TOO_LOW },
                 { 2, SingleStarDetectorAlgorithmT::ResultT::StatusT::UNEXPECTED_STAR_COUNT }
         }),
     poi, numStarsDetected, starDetectorResult)
{
    SingleStarDetectorAlgorithmT singleStarDetectorAlgorithm(SNR_LIMIT, mStarWindowSize);
    auto result = singleStarDetectorAlgorithm.detect(mTestImage, poi);

    BOOST_CHECK_CLOSE( result.getSnrLimit(), SNR_LIMIT, 0.001F );
    BOOST_CHECK_EQUAL( result.getNumStarsDetected(), numStarsDetected );
    BOOST_CHECK_EQUAL( result.getStatus(), starDetectorResult );
}

/**
 * Check for exception in case no image is specified.
 */
BOOST_AUTO_TEST_CASE(single_star_detector_test_empty_image)
{
    SingleStarDetectorAlgorithmT singleStarDetectorAlgorithm(SNR_LIMIT, mStarWindowSize);
    BOOST_CHECK_THROW(singleStarDetectorAlgorithm.detect(nullptr, PointT<float>(100.0F, 100.0F)), SingleStarDetectorExceptionT);
}

BOOST_AUTO_TEST_SUITE_END()
