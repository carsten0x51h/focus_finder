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

#include "../../common/include/image.h"
#include "../../common/include/star_cluster_algorithm.h"


BOOST_AUTO_TEST_SUITE(star_cluster_algorithm_tests)

/**
 * Two single pixels which have a distance > 2 pixels should be
 * clustered as two separate segments. The test image contains
 * only two white pixels at positions: (x=12, y=23) and (x=23, x=9).
 */
BOOST_AUTO_TEST_CASE(star_cluster_algorithm_two_pixels_test_image)
{
    ImageT binaryImg("test_data/test_image_9.tif");

    StarClusterAlgorithmT starClusterAlgorithm(
            2 /*defines the allowed number of dark pixels between two white pixels until they form a cluster*/);
    std::list<PixelClusterT> clusters = starClusterAlgorithm.cluster(binaryImg);

    BOOST_TEST(clusters.size() == 2);

    const auto & pixelCluster1 = clusters.begin();
    const auto & pixelCluster2 = std::next(clusters.begin());

    BOOST_TEST(pixelCluster1->size() == 1);
    BOOST_TEST(pixelCluster2->size() == 1);

    PixelPosT c1p1 = *pixelCluster1->begin();
    BOOST_TEST(c1p1.x() == 12);
    BOOST_TEST(c1p1.y() == 23);

    PixelPosT c2p1 = *pixelCluster2->begin();
    BOOST_TEST(c2p1.x() == 23);
    BOOST_TEST(c2p1.y() == 9);
}

/**
 * A group of connected pixels with less than 2 0 pixels
 * in between should be detected as one segment.
 */
BOOST_AUTO_TEST_CASE(star_cluster_algorithm_single_segment_test_image)
{
    ImageT binaryImg("test_data/test_image_10.tif");

    StarClusterAlgorithmT starClusterAlgorithm(
            2 /*defines the allowed number of dark pixels between two white pixels until they form a cluster*/);
    std::list<PixelClusterT> clusters = starClusterAlgorithm.cluster(binaryImg);


    // Expect 1 pixel cluster (see image)
    BOOST_TEST(clusters.size() == 1);

    const auto & pixelCluster1 = clusters.begin();

    BOOST_TEST(pixelCluster1->size() == 71); // Expect 71 pixels which are set (counted in image)


    // Check that all expected pixels are in the segment
    std::list<PixelPosT> expectedPixels = {
            PixelPosT(9, 16), PixelPosT(10, 14), PixelPosT(10, 15), PixelPosT(10, 16), PixelPosT(11, 14),
            PixelPosT(11, 15), PixelPosT(11, 16), PixelPosT(11, 17), PixelPosT(10, 12), PixelPosT(11, 12),
            PixelPosT(11, 13), PixelPosT(12, 12), PixelPosT(12, 13), PixelPosT(12, 14), PixelPosT(12, 15),
            PixelPosT(12, 16), PixelPosT(12, 17), PixelPosT(13, 12), PixelPosT(13, 13), PixelPosT(13, 14),
            PixelPosT(13, 15), PixelPosT(13, 16), PixelPosT(13, 17), PixelPosT(11, 10), PixelPosT(11, 11),
            PixelPosT(12, 10), PixelPosT(12, 11), PixelPosT(13, 10), PixelPosT(13, 11), PixelPosT(14, 10),
            PixelPosT(14, 11), PixelPosT(14, 12), PixelPosT(14, 13), PixelPosT(14, 14), PixelPosT(14, 15),
            PixelPosT(14, 16), PixelPosT(14, 17), PixelPosT(15, 10), PixelPosT(15, 11), PixelPosT(15, 12),
            PixelPosT(15, 13), PixelPosT(15, 14), PixelPosT(15, 15), PixelPosT(15, 16), PixelPosT(15, 19),
            PixelPosT(10, 9), PixelPosT(11, 9), PixelPosT(12, 9), PixelPosT(13, 9), PixelPosT(15, 9), PixelPosT(16, 9),
            PixelPosT(16, 10), PixelPosT(16, 11), PixelPosT(16, 12), PixelPosT(16, 13), PixelPosT(16, 14),
            PixelPosT(16, 15), PixelPosT(16, 16), PixelPosT(16, 17), PixelPosT(16, 18), PixelPosT(17, 11),
            PixelPosT(17, 12), PixelPosT(17, 13), PixelPosT(17, 14), PixelPosT(17, 15), PixelPosT(17, 16),
            PixelPosT(18, 11), PixelPosT(18, 12), PixelPosT(18, 13), PixelPosT(18, 14), PixelPosT(19, 15)
    };

    BOOST_TEST(*pixelCluster1 == expectedPixels, boost::test_tools::per_element());
}


/**
 * Try to detect three independent segments.
 */
BOOST_AUTO_TEST_CASE(star_cluster_algorithm_three_segments_test)
{
    ImageT binaryImg("test_data/test_image_11.tif");

    StarClusterAlgorithmT starClusterAlgorithm(
            2 /*defines the allowed number of dark pixels between two white pixels until they form a cluster*/);
    std::list<PixelClusterT> clusters = starClusterAlgorithm.cluster(binaryImg);

    BOOST_TEST(clusters.size() == 3);

    //
    // Check that all expected pixels are in respective segments.
    //
    // Segment 1
    const auto & pixelCluster1 = clusters.begin();

    BOOST_TEST(pixelCluster1->size() == 12);

    std::list<PixelPosT> expectedPixelsCluster1 = {
            PixelPosT(8, 8), PixelPosT(8, 9), PixelPosT(9, 8), PixelPosT(9, 9), PixelPosT(9, 10), PixelPosT(10, 8),
            PixelPosT(10, 9), PixelPosT(10, 10), PixelPosT(11, 7), PixelPosT(11, 8), PixelPosT(12, 6), PixelPosT(13, 5)
    };

    BOOST_TEST(*pixelCluster1 == expectedPixelsCluster1, boost::test_tools::per_element());

    // Segment 2
    const auto & pixelCluster2 = std::next(clusters.begin());

    BOOST_TEST(pixelCluster2->size() == 8);

    std::list<PixelPosT> expectedPixelsCluster2 = {
            PixelPosT(11, 23), PixelPosT(11, 25), PixelPosT(12, 22), PixelPosT(12, 23), PixelPosT(12, 24),
            PixelPosT(13, 22), PixelPosT(13, 23), PixelPosT(14, 24)
    };

    BOOST_TEST(*pixelCluster2 == expectedPixelsCluster2, boost::test_tools::per_element());

    // Segment 3
    const auto & pixelCluster3 = std::next(clusters.begin(), 2);

    BOOST_TEST(pixelCluster3->size() == 9);
    std::list<PixelPosT> expectedPixelsCluster3 = {
            PixelPosT(22, 9), PixelPosT(22, 10), PixelPosT(23, 9), PixelPosT(23, 10), PixelPosT(23, 11),
            PixelPosT(24, 8), PixelPosT(24, 10), PixelPosT(24, 12), PixelPosT(25, 13)
    };

    BOOST_TEST(*pixelCluster3 == expectedPixelsCluster3, boost::test_tools::per_element());
}


/**
 * Test different cluster radii.
 */
BOOST_AUTO_TEST_CASE(star_cluster_algorithm_cluster_radius_test)
{
    ImageT binaryImg("test_data/test_image_12.tif");

    StarClusterAlgorithmT starClusterAlgorithm1(
            2 /*defines the allowed number of dark pixels between two white pixels until they form a cluster*/);

    BOOST_TEST(starClusterAlgorithm1.cluster(binaryImg).size() == 2);


    StarClusterAlgorithmT starClusterAlgorithm2(
            3 /*defines the allowed number of dark pixels between two white pixels until they form a cluster*/);

    BOOST_TEST(starClusterAlgorithm2.cluster(binaryImg).size() == 1);
}


/**
 * Black image - no cluster segments expected.
 */
BOOST_AUTO_TEST_CASE(star_cluster_algorithm_no_cluster_test)
{
    ImageT binaryImg("test_data/test_image_2.tif"); // black image

    StarClusterAlgorithmT starClusterAlgorithm(
            2 /*defines the allowed number of dark pixels between two white pixels until they form a cluster*/);

    // Expect that no cluster was found
    BOOST_TEST(starClusterAlgorithm.cluster(binaryImg).size() == 0);
}

BOOST_AUTO_TEST_SUITE_END();
