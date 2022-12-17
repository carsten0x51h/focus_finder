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

#include <range/v3/view/all.hpp>
#include <boost/test/unit_test.hpp>

#include "../../common/include/pipeline/algorithm/files.h"

BOOST_AUTO_TEST_SUITE(pipeline_files_tests)

using namespace AstroImagePipeline;
using namespace ranges;


/**
 * NOTE: A Data-testcase was not possible here because having a nested container inside the bdata spec.
 *       didn't compile.
 *
 * See https://stackoverflow.com/questions/63115900/c20-ranges-view-to-vector
 * See https://stackoverflow.com/questions/64938645/transform-temporary-vector-with-range-v3
 */
BOOST_AUTO_TEST_CASE(pipeline_files_filenames_with_no_spaces_test)
{
    const std::vector<std::string> filePaths { "test_data/image_processing_pipeline/files" };
    const std::vector<std::string> expectedFilenames { "test_data/image_processing_pipeline/files/tiff_file_1.tiff",
                                                       "test_data/image_processing_pipeline/files/tiff_file_2.tiff" };

    auto result = filePaths | files("(.*\\.tiff)") | actions::join | actions::sort | to<std::vector>();

    BOOST_TEST(result == expectedFilenames);
}


BOOST_AUTO_TEST_CASE(pipeline_files_filenames_with_spaces_test)
{
    const std::vector<std::string> filePaths { "test_data/image_processing_pipeline/files" };
    const std::vector<std::string> expectedFilenames { "test_data/image_processing_pipeline/files/fits file with spaces 1.fits",
                                                       "test_data/image_processing_pipeline/files/fits file with spaces 2.fits",
                                                       "test_data/image_processing_pipeline/files/fits_file_1.fits",
                                                       "test_data/image_processing_pipeline/files/fits_file_2.fits" };

    auto result = filePaths | files("(.*\\.fits)") | actions::join | actions::sort | to<std::vector>();

    BOOST_TEST(result == expectedFilenames);
}


BOOST_AUTO_TEST_CASE(pipeline_files_non_exising_extension_test)
{
    const std::vector<std::string> filePaths { "test_data/image_processing_pipeline/files" };
    const std::vector<std::string> expectedFilenames { };

    auto result = filePaths | files("(.*\\.xyz)") | actions::join | actions::sort | to<std::vector>();

    BOOST_TEST(result == expectedFilenames);
}

BOOST_AUTO_TEST_CASE(pipeline_files_no_extension_filter_test)
{
    const std::vector<std::string> filePaths { "test_data/image_processing_pipeline/files" };
    const std::vector<std::string> expectedFilenames {
            "test_data/image_processing_pipeline/files/fits file with spaces 1.fits",
            "test_data/image_processing_pipeline/files/fits file with spaces 2.fits",
            "test_data/image_processing_pipeline/files/fits_file_1.fits",
            "test_data/image_processing_pipeline/files/fits_file_2.fits",
            "test_data/image_processing_pipeline/files/noext_file_1",
            "test_data/image_processing_pipeline/files/noext_file_2",
            "test_data/image_processing_pipeline/files/tiff_file_1.tiff",
            "test_data/image_processing_pipeline/files/tiff_file_2.tiff",
            "test_data/image_processing_pipeline/files/txt_file_1.txt",
            "test_data/image_processing_pipeline/files/txt_file_2.txt"
    };

    auto result = filePaths | files() | actions::join | actions::sort | to<std::vector>();

    BOOST_TEST(result == expectedFilenames);
}

BOOST_AUTO_TEST_SUITE_END();
