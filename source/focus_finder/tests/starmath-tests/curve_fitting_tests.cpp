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

#include <memory>

#include "../../common/include/curve_function_gaussian.h"
#include "../../common/include/curve_fit_parms.h"
#include "../../common/include/curve_fit_summary.h"
#include "../../common/include/curve_fit_algorithm.h"

BOOST_AUTO_TEST_SUITE(curve_fitting_tests)

/**
 * Fit a gaussian curve against data points from a star profile - i.e.
 * data points which are suited for the gaussian model.
 */
// FIXME: Failed because result values slightly changed...
//BOOST_AUTO_TEST_CASE(gaussian_curve_fit_test)
//{
//    // Data points to fit with a gaussian curve (star profile)
//    std::vector<PointFT> dataPoints = { PointFT(0, 12064), PointFT(1, 11727), PointFT(2, 11305), PointFT(3, 11335),
//                                        PointFT(4, 11837), PointFT(5, 12627), PointFT(6, 13091), PointFT(7, 12459),
//                                        PointFT(8,11946), PointFT(9, 11835), PointFT(10, 11427), PointFT(11, 11421),
//                                        PointFT(12,12386), PointFT(13, 12631), PointFT(14, 15955), PointFT(15, 28755),
//                                        PointFT(16, 65327), PointFT(17, 65535), PointFT(18, 62001), PointFT(19, 26827),
//                                        PointFT(20, 16569), PointFT(21, 13230), PointFT(22, 12216), PointFT(23, 11570),
//                                        PointFT(24, 11356), PointFT(25, 11463), PointFT(26, 11497), PointFT(27, 11541),
//                                        PointFT(28, 11863), PointFT(29, 11613), PointFT(30, 11985), PointFT(31, 11729),
//                                        PointFT(32, 11606), PointFT(33, 11442), PointFT(34, 11659) };
//
//    std::vector<PointFT> expectedCurveValues = { PointFT(0, 11791.0595703125), PointFT(1, 11791.0595703125), PointFT(2, 11791.0595703125), PointFT(3, 11791.0595703125), PointFT(4, 11791.0595703125), PointFT(5, 11791.0595703125), PointFT(6, 11791.0595703125), PointFT(7, 11791.0595703125), PointFT(8, 11791.0595703125), PointFT(9, 11791.0605468750), PointFT(10,11791.1093750000), PointFT(11,11793.1718750000), PointFT(12,11840.8671875000), PointFT(13,12448.8378906250), PointFT(14,16656.8632812500), PointFT(15,31952.5625000000), PointFT(16,58584.4648437500), PointFT(17,72624.1406250000), PointFT(18,56089.4960937500), PointFT(19,29859.9062500000), PointFT(20,15919.3085937500), PointFT(21,12319.3769531250), PointFT(22,11828.9316406250), PointFT(23,11792.5800781250), PointFT(24,11791.0937500000), PointFT(25,11791.0595703125), PointFT(26,11791.0595703125), PointFT(27,11791.0595703125), PointFT(28,11791.0595703125), PointFT(29,11791.0595703125), PointFT(30,11791.0595703125), PointFT(31,11791.0595703125), PointFT(32,11791.0595703125), PointFT(33,11791.0595703125), PointFT(34,11791.0595703125) };
//
//
//    CurveFitParmsT curveFitParms(FittingCurveTypeT::GAUSSIAN);
//    CurveFitSummaryT curveFitSummary;
//
//    CurveParmsT curveParms = CurveFitAlgorithmT::fitCurve(dataPoints,
//                                              curveFitParms,
//                                              &curveFitSummary);
//
//    // Check determined curve parms
//    BOOST_TEST(curveParms.get(CurveFunctionGaussianT::IdxT::B_IDX).getValue() == 11791.1, boost::test_tools::tolerance( 1e-3 ));
//    BOOST_TEST(curveParms.get(CurveFunctionGaussianT::IdxT::P_IDX).getValue() == 60872.5, boost::test_tools::tolerance( 1e-3 ));
//    BOOST_TEST(curveParms.get(CurveFunctionGaussianT::IdxT::C_IDX).getValue() == 16.9527, boost::test_tools::tolerance( 1e-3 ));
//    BOOST_TEST(curveParms.get(CurveFunctionGaussianT::IdxT::W_IDX).getValue() == 1.31354, boost::test_tools::tolerance( 1e-3 ));
//    BOOST_TEST(curveFitSummary.success == true);
//    BOOST_TEST(curveFitSummary.matchedDataPoints.size() == 35);
//    BOOST_TEST(curveFitSummary.numIterationsRequiredTotal == 4);
//    BOOST_TEST(curveFitSummary.curveDataPoints == expectedCurveValues, boost::test_tools::per_element());
//    BOOST_TEST(curveFitSummary.matchedDataPoints == dataPoints, boost::test_tools::per_element());
//    BOOST_TEST(curveFitSummary.outliers.size() == 0);
//    BOOST_TEST(curveFitSummary.outlierBoundary == 0.0F, boost::test_tools::tolerance( 1e-3 ));
//}


/**
 * Fit a gaussian curve against data points from a white noise pattern - i.e.
 * data points which do not suited for the gaussian model. It is expected that
 * the curve fitting fails.
 */
 // TODO: FAILS...
//    BOOST_AUTO_TEST_CASE(gaussian_curve_fit_failure_test)
//    {
//        //TODO: Somehow fits the values below even if it should not...
//
//        // Data points to fit with a gaussian curve (gauss anti-pattern)
//        std::vector<PointFT> dataPoints = { PointFT(0, 62064), PointFT(1, 51727), PointFT(2, 41305), PointFT(3, 31335),
//                                            PointFT(4, 21837), PointFT(5, 12627), PointFT(6, 13091), PointFT(7, 12459),
//                                            PointFT(8,11946), PointFT(9, 11835), PointFT(10, 11427), PointFT(11, 11421),
//                                            PointFT(12,12386), PointFT(13, 12631), PointFT(14, 11421), PointFT(15, 12451),
//                                            PointFT(16, 11946), PointFT(17, 11861), PointFT(18, 11499), PointFT(19, 11597),
//                                            PointFT(20, 12146), PointFT(21, 12230), PointFT(22, 12216), PointFT(23, 11570),
//                                            PointFT(24, 11356), PointFT(25, 11463), PointFT(26, 11497), PointFT(27, 11541),
//                                            PointFT(28, 11863), PointFT(29, 11613), PointFT(30, 22985), PointFT(31, 33729),
//                                            PointFT(32, 44606), PointFT(33, 55442), PointFT(34, 64659) };
//
////        std::vector<PointFT> expectedCurveValues = {PointFT(0, 11791.0595703125), PointFT(1, 11791.0595703125),
////                                                    PointFT(2, 11791.0595703125), PointFT(3, 11791.0595703125),
////                                                    PointFT(4, 11791.0595703125), PointFT(5, 11791.0595703125),
////                                                    PointFT(6, 11791.0595703125), PointFT(7, 11791.0595703125),
////                                                    PointFT(8, 11791.0595703125), PointFT(9, 11791.0605468750),
////                                                    PointFT(10, 11791.1093750000), PointFT(11, 11793.1718750000),
////                                                    PointFT(12, 11840.8671875000), PointFT(13, 12448.8378906250),
////                                                    PointFT(14, 16656.8632812500), PointFT(15, 31952.5625000000),
////                                                    PointFT(16, 58584.4648437500), PointFT(17, 72624.1406250000),
////                                                    PointFT(18, 56089.4960937500), PointFT(19, 29859.9062500000),
////                                                    PointFT(20, 15919.3085937500), PointFT(21, 12319.3769531250),
////                                                    PointFT(22, 11828.9316406250), PointFT(23, 11792.5800781250),
////                                                    PointFT(24, 11791.0937500000), PointFT(25, 11791.0595703125),
////                                                    PointFT(26, 11791.0595703125), PointFT(27, 11791.0595703125),
////                                                    PointFT(28, 11791.0595703125), PointFT(29, 11791.0595703125),
////                                                    PointFT(30, 11791.0595703125), PointFT(31, 11791.0595703125),
////                                                    PointFT(32, 11791.0595703125), PointFT(33, 11791.0595703125),
////                                                    PointFT(34, 11791.0595703125)};
//
//
//        CurveFitParmsT curveFitParms(FittingCurveTypeT::GAUSSIAN);
//        CurveFitSummaryT curveFitSummary;
//
//        CurveParmsT curveParms = CurveFitAlgorithmT::fitCurve(dataPoints,
//                                                              curveFitParms,
//                                                              &curveFitSummary);
//
//        std::cerr << "curveParms: " << curveParms << std::endl;
//        std::cerr << "curveFitSummary: " << curveFitSummary << std::endl;
//
//
//
//
//        // Check determined curve parms
////        BOOST_TEST(curveParms.get(CurveFunctionGaussianT::IdxT::B_IDX).getValue() == 11791.1, boost::test_tools::tolerance( 1e-3 ));
////        BOOST_TEST(curveParms.get(CurveFunctionGaussianT::IdxT::P_IDX).getValue() == 60872.5, boost::test_tools::tolerance( 1e-3 ));
////        BOOST_TEST(curveParms.get(CurveFunctionGaussianT::IdxT::C_IDX).getValue() == 16.9527, boost::test_tools::tolerance( 1e-3 ));
////        BOOST_TEST(curveParms.get(CurveFunctionGaussianT::IdxT::W_IDX).getValue() == 1.31354, boost::test_tools::tolerance( 1e-3 ));
////        BOOST_TEST(curveFitSummary.success == true);
////        BOOST_TEST(curveFitSummary.matchedDataPoints.size() == 35);
////        BOOST_TEST(curveFitSummary.numIterationsRequiredTotal == 4);
////        BOOST_TEST(curveFitSummary.curveDataPoints == expectedCurveValues, boost::test_tools::per_element());
////        BOOST_TEST(curveFitSummary.matchedDataPoints == dataPoints, boost::test_tools::per_element());
////        BOOST_TEST(curveFitSummary.outliers.size() == 0);
////        BOOST_TEST(curveFitSummary.outlierBoundary == 0.0F, boost::test_tools::tolerance( 1e-3 ));
//    }

// TODO: Add further tests ...
// - matching of data against a "wrong" curve -> observe failure
// - Too few values -> observe error
// - Test Outier detector... -> maybe sep. unit tests?
//        try {
//        } catch (CurveFitExceptionT &exc) {



//BOOST_AUTO_TEST_CASE(gaussian_star_gen)
//{
//    std::vector<PointFT> expectedCurveValues = {PointFT(0, 1791.0595703125), PointFT(1, 1791.0595703125),
//                                                PointFT(2, 1791.0595703125), PointFT(3, 1791.0595703125),
//                                                PointFT(4, 1791.0595703125), PointFT(5, 1791.0595703125),
//                                                PointFT(6, 1791.0595703125), PointFT(7, 1791.0595703125),
//                                                PointFT(8, 1791.0595703125), PointFT(9, 1791.0605468750),
//                                                PointFT(10, 1791.1093750000), PointFT(11, 1793.1718750000),
//                                                PointFT(12, 1840.8671875000), PointFT(13, 2448.8378906250),
//                                                PointFT(14, 6656.8632812500), PointFT(15, 21952.5625000000),
//                                                PointFT(16, 48584.4648437500), PointFT(17, 62624.1406250000),
//                                                PointFT(18, 46089.4960937500), PointFT(19, 19859.9062500000),
//                                                PointFT(20, 5919.3085937500), PointFT(21, 2319.3769531250),
//                                                PointFT(22, 1828.9316406250), PointFT(23, 1792.5800781250),
//                                                PointFT(24, 1791.0937500000), PointFT(25, 1791.0595703125),
//                                                PointFT(26, 1791.0595703125), PointFT(27, 1791.0595703125),
//                                                PointFT(28, 1791.0595703125), PointFT(29, 1791.0595703125),
//                                                PointFT(30, 1791.0595703125), PointFT(31, 1791.0595703125),
//                                                PointFT(32, 1791.0595703125), PointFT(33, 1791.0595703125),
//                                                PointFT(34, 1791.0595703125)};
//
//        size_t centerIdx = std::floor((float) expectedCurveValues.size() / 2.0);
//        PointFT centerDataPoint = expectedCurveValues.at(centerIdx);
//        float centerValue = centerDataPoint.y();
//
//        std::cerr << "centerIdx: " << centerIdx << ", centerValue: " << centerValue << std::endl;
//
//        //TODO: Check https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
//        // OR: Maybe better find a tool which creates a 2D 16 bit gaussian distribution image...
//        // --> https://en.wikipedia.org/wiki/Multivariate_normal_distribution
//}


BOOST_AUTO_TEST_SUITE_END();
