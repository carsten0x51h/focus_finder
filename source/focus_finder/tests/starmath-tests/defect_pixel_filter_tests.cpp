// /*****************************************************************************
//  *
//  *  FoFi - A free, automatic telescope focus finer.
//  *
//  *  Copyright(C) 2019 Carsten Schmitt <c [at] lost-infinity.com>
//  *
//  *  More info on https://www.lost-infinity.com
//  *
//  *  This program is free software ; you can redistribute it and/or modify
//  *  it under the terms of the GNU General Public License as published by
//  *  the Free Software Foundation ; either version 2 of the License, or
//  *  (at your option) any later version.
//  *
//  *  This program is distributed in the hope that it will be useful,
//  *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
//  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  *  GNU General Public License for more details.
//  *
//  *  You should have received a copy of the GNU General Public License
//  *  along with this program ; if not, write to the Free Software
//  *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//  *
//  ****************************************************************************/

// #include <boost/test/unit_test.hpp>

// #include "../../common/include/image.h"

// BOOST_AUTO_TEST_SUITE(defect_pixel_detector_tests)

// /**
//  *
//  */
// BOOST_AUTO_TEST_CASE(cold_pixel_detector_test)
// {
//     ImageT expectedResultImage(5,5,1,1,4); // 5x5 - bg value 4
//     expectedResultImage(2,2) = 102.0F;

//     // TODO: How to design th API of a hot-pixel handler?
//     //       -> What should a hot-pixel handler do?
//     //       -> There are two steps.... A hot-pixel filter would just identify and remove the hot-pixels... (typical
//     //          median 3x3 filter. However, for some UI just detecting the hot-pixels might be interesting -
//     //          e.g. to for visualizing the hot-pixels before (potentially selective) removal in a second step.
//     //          The pipeline will certainly just detect and remove the hot-pixels.
//     //          Actually there is no special state which needs to be hold. Therefore, a class instance is not needed.
//     //          In fact the methods could just be moved to some namespace.
//     std::list<PosXY> r = HotpixelHandlerT::detect_hotpixels(img);                         // Detects hotpixels - Returns list of detected hot-pixels
//     modified_img?/new_img = HotpixelHandlerT::remove_pixels(img, std::list<PosXY>);    // Removes hotpixels at given postions - Returns mod. img?

    
//     // OR
//     modified_img?/new_img = HotpixelHandlerT::remove_hotpixels(img);                      // Detects and removes hotpixels - Returns mod. img?

//     const std::vector<std::string> imageFilenames {
//             "test_data/defect_pixel_filter/test_image_cold_pixel_removal_25x25.tiff",
//     };

//     auto resultImagePtr = imageFilenames
//                           | images()
//                           | add(imageToAddPtr)
//                           | to<std::vector>();

//     // NOTE: Exactly one image is expected
//     const ImageT & resultImage = *(resultImagePtr.at(0));

//     BOOST_TEST(resultImage == expectedResultImage);
// }

// BOOST_AUTO_TEST_SUITE_END();
