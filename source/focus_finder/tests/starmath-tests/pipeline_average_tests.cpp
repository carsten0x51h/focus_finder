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

#include <range/v3/range/conversion.hpp>

#include <boost/test/unit_test.hpp>

#include "../../common/include/pipeline/adapter/images.h"
#include "../../common/include/pipeline/algorithm/average.h"

BOOST_AUTO_TEST_SUITE(pipeline_average_tests)

using namespace AstroImagePipeline;
using namespace ranges;

/**
 *
 */
BOOST_AUTO_TEST_CASE(pipeline_average_1_test)
{
    ImageT expectedResultImage(5,5,1,1,250); // 5x5 - bg value 250
    expectedResultImage(2,2) = 16571.25F;
    expectedResultImage(0,4) = 8187.5F;
    expectedResultImage(4,0) = 8187.5F;

    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/average/test_image_average_1_5x5.tiff",
            "test_data/image_processing_pipeline/average/test_image_average_2_5x5.tiff",
            "test_data/image_processing_pipeline/average/test_image_average_3_5x5.tiff",
            "test_data/image_processing_pipeline/average/test_image_average_4_5x5.tiff"
    };

    BOOST_TEST(*average(imageFilenames | images()) == expectedResultImage);
}

BOOST_AUTO_TEST_SUITE_END();
