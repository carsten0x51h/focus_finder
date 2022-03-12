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
#include <boost/test/tools/floating_point_comparison.hpp>

#include <set>

#include "../../common/include/point.h"

BOOST_AUTO_TEST_SUITE(point_tests)

/**
 * Test if the point equal operator works for int type.
 */
BOOST_AUTO_TEST_CASE(point_int_equal_test)
{
    PointT<int> p1(10,15);
    PointT<int> p2(10,15);
    PointT<int> p3(11,15);
    PointT<int> p4(10,16);
    PointT<int> p5(11,16);
    PointT<int> p6(11,-16);
    PointT<int> p7(11,-16);

    BOOST_TEST((p1 == p1) == true);
    BOOST_TEST((p1 == p2) == true);
    BOOST_TEST((p1 == p3) == false);
    BOOST_TEST((p1 == p4) == false);
    BOOST_TEST((p1 == p5) == false);
    BOOST_TEST((p1 == p6) == false);
    BOOST_TEST((p6 == p7) == true);
}

/**
 * Test if the point equal operator works for unsigned int type.
 * This test is mainly done to exclude any problems which might
 * occur because of subtractions where the result could get negative
 * and which is then stored accidentally in an unsigned int.
 */
BOOST_AUTO_TEST_CASE(point_unsigned_int_equal_test)
{
    PointT<unsigned int> p1(10,15);
    PointT<unsigned int> p2(10,15);
    PointT<unsigned int> p3(11,15);
    PointT<unsigned int> p4(10,16);
    PointT<unsigned int> p5(11,16);

    BOOST_TEST((p1 == p1) == true);
    BOOST_TEST((p1 == p2) == true);
    BOOST_TEST((p1 == p3) == false);
    BOOST_TEST((p1 == p4) == false);
    BOOST_TEST((p1 == p5) == false);
}


/**
 * Test if the less operator works for the int type.
 */
BOOST_AUTO_TEST_CASE(point_int_less_test)
{
    PointT<int> p1(10,15);
    PointT<int> p2(10,15);
    PointT<int> p3(11,16);
    PointT<int> p4(11,15);
    PointT<int> p5(10,16);
    PointT<int> p6(11,-16);
    PointT<int> p7(11,-16);

    BOOST_TEST((p1 < p2) == false);
    BOOST_TEST((p1 < p3) == true);
    BOOST_TEST((p1 < p4) == true);
    BOOST_TEST((p1 < p5) == true);
    BOOST_TEST((p5 < p6) == true);
    BOOST_TEST((p6 < p7) == false);
}


/**
 * Test if the less operator works for the unsigned int type.
 * This test is mainly done to exclude any problems which might
 * occur because of subtractions where the result could get negative
 * and which is then stored accidentally in an unsigned int.
 */
BOOST_AUTO_TEST_CASE(point_unsigned_int_less_test)
{
    PointT<unsigned int> p1(10,15);
    PointT<unsigned int> p2(10,15);
    PointT<unsigned int> p3(11,16);
    PointT<unsigned int> p4(11,15);
    PointT<unsigned int> p5(10,16);

    BOOST_TEST((p1 < p2) == false);
    BOOST_TEST((p1 < p3) == true);
    BOOST_TEST((p1 < p4) == true);
    BOOST_TEST((p1 < p5) == true);
}


/**
 * Test if adding the point to an STD set behaves as expected
 * (verifying the correct functioning of overloaded operators).
 */
BOOST_AUTO_TEST_CASE(point_fixed_add_to_set_test)
{
    PointT<int> p1(10,15);
    PointT<int> p2(11,15);
    PointT<int> p3(10,16);
    PointT<int> p4(11,16);
    PointT<int> p5(10,15);

    std::set <PointT<int>> mySet;

    mySet.insert(p1);
    BOOST_TEST(mySet.size() == 1);

    mySet.insert(p2);
    BOOST_TEST(mySet.size() == 2);

    mySet.insert(p3);
    BOOST_TEST(mySet.size() == 3);

    mySet.insert(p4);
    BOOST_TEST(mySet.size() == 4);

    mySet.insert(p5);
    BOOST_TEST(mySet.size() == 4);

    mySet.insert(p1);
    BOOST_TEST(mySet.size() == 4);
}

/**
 * Try to find a point from within a set (testing of
 * overloaded operators).
 */
BOOST_AUTO_TEST_CASE(point_fixed_find_in_set_test)
{
    std::set<PointT<int>> pointSet;

    PointT<int> p1(10,15);
    PointT<int> p2(11,15);
    PointT<int> p3(10,16);
    PointT<int> p4(17,18);

    pointSet.insert(p1);
    pointSet.insert(p2);
    pointSet.insert(p3);

    BOOST_TEST(pointSet.size() == 3);

    bool foundP2 = (pointSet.find(p2) != pointSet.end());
    BOOST_TEST(foundP2 == true);

    bool foundP4 = (pointSet.find(p4) != pointSet.end());
    BOOST_TEST(foundP4 == false);
}

/**
 * Try to set and get x and y values.
 */
BOOST_AUTO_TEST_CASE(point_setter_getter_test)
{
    PointT<int> p1(10,15);

    BOOST_TEST(p1.x() == 10);
    BOOST_TEST(p1.y() == 15);

    p1.setX(11);
    BOOST_TEST(p1.x() == 11);

    p1.setY(16);
    BOOST_TEST(p1.y() == 16);
}

/**
 * Test default point initialization. The default
 * values for x and y should be (0,0).
 */
BOOST_AUTO_TEST_CASE(point_default_init_test)
{
    PointT<int> p1;

    BOOST_TEST(p1.x() == 0);
    BOOST_TEST(p1.y() == 0);
}

/**
 * Test specialized (overloaded) comparison operator
 * for float.
 */
BOOST_AUTO_TEST_CASE(point_compare_float_points_test)
{
    PointT<float> p1(1.00000000F, 2.00000000F);
    PointT<float> p2(1.00000000F, 2.00000001F);
    PointT<float> p3(1.00000001F, 2.00000001F);
    PointT<float> p4(1.00000001F, 2.001F);

    BOOST_TEST(p1 == p2);
    BOOST_TEST(p2 == p3);
    BOOST_TEST(!(p3 == p4));
}

/**
 * Test specialized (overloaded) comparison operator
 * for double.
 */
BOOST_AUTO_TEST_CASE(point_compare_double_points_test)
{
    PointT<double> p1(1.0000000000000000, 2.0000000000000000);
    PointT<double> p2(1.0000000000000000, 2.0000000000000001);
    PointT<double> p3(1.0000000000000001, 2.0000000000000001);
    PointT<double> p4(1.0000000000000001, 2.000000000001);

    BOOST_TEST(p1 == p2);
    BOOST_TEST(p2 == p3);
    BOOST_TEST(!(p3 == p4));
}

BOOST_AUTO_TEST_SUITE_END();
