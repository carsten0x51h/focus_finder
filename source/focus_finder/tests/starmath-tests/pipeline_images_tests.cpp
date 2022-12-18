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

//#include <range/v3/view/all.hpp>
#include <boost/test/unit_test.hpp>

#include "../../common/include/pipeline/adapter/images.h"

BOOST_AUTO_TEST_SUITE(pipeline_images_tests)

using namespace AstroImagePipeline;
using namespace ranges;


BOOST_AUTO_TEST_CASE(pipeline_images_tiff_test)
{
    const std::vector<std::string> imageFilenames {
            "test_data/image_processing_pipeline/images/test_image_tiff_1_65x85.tiff",
            "test_data/image_processing_pipeline/images/test_image_tiff_2_65x85.tiff",
    };

    for (const auto & img : imageFilenames | images()) {
        BOOST_TEST(img->width() == 65);
        BOOST_TEST(img->height() == 85);
    }
}

// TODO: fits...
//{ "test_data/image_processing_pipeline/images/test_image_fits_1_45x47.fits", 45, 47 },
//{ "test_data/image_processing_pipeline/images/test_image_fits_2_45x47.fits", 45, 47 }

BOOST_AUTO_TEST_SUITE_END();
