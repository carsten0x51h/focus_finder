///*****************************************************************************
// *
// *  FoFi - A free, automatic telescope focus finder.
// *
// *  Copyright(C) 2019 Carsten Schmitt <c [at] lost-infinity.com>
// *
// *  More info on https://www.lost-infinity.com
// *
// *  This program is free software ; you can redistribute it and/or modify
// *  it under the terms of the GNU General Public License as published by
// *  the Free Software Foundation ; either version 2 of the License, or
// *  (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// *  GNU General Public License for more details.
// *
// *  You should have received a copy of the GNU General Public License
// *  along with this program ; if not, write to the Free Software
// *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
// *
// ****************************************************************************/
//
////#include <ranges>
//
//#include <boost/test/unit_test.hpp>
////#include <boost/test/tools/floating_point_comparison.hpp>
//
////#include <boost/range/numeric.hpp>
////#include <boost/range/adaptor/filtered.hpp>
////#include <boost/range/adaptor/transformed.hpp>
//
////#include <boost/range/adaptors.hpp>
////#include <boost/range/algorithm.hpp>
//
//#include "../../common/include/image.h"
//#include "../../common/include/thresholding_algorithm_factory.h"
//#include "../../common/include/centroid_algorithm_factory.h"
//#include "../../common/include/pipeline/adapter/images.h"
//#include "../../common/include/pipeline/adapter/subtract_background.h"
//#include "../../common/include/pipeline/adapter/scale.h"
//#include "../../common/include/pipeline/adapter/center_on_star.h"
//#include "../../common/include/pipeline/adapter/crop.h"
//
////#include "../../common/include/pipeline/algorithm/files.h"
//#include "../../common/include/pipeline/algorithm/average.h"
//
//
////#include "../../common/include/star_analysis.h"
////#include "../../common/include/hfd.h"
//
//
//BOOST_AUTO_TEST_SUITE(astro_image_processing_pipeline_tests)
//
//
//
////BOOST_AUTO_TEST_CASE(pipeline_subtract_test)
////{
////    std::vector<std::string> imageNames = {
////        "test_data/image_processing_pipeline/subtract/test_image_all_values_0_65x85.tiff",
////        "test_data/image_processing_pipeline/subtract/test_image_all_values_313_65x85.tiff",
////        "test_data/image_processing_pipeline/subtract/test_image_all_values_353_65x85.tiff",
////        "test_data/image_processing_pipeline/subtract/test_image_all_values_541_65x85.tiff"
////    };
////
////    std::shared_ptr<ImageT> averageImg = average(imageNames | images());
////
////}
////
////BOOST_AUTO_TEST_CASE(pipeline_average_test)
////{
////    std::vector<std::string> imageNames = {
////            "test_data/image_processing_pipeline/subtract/test_image_all_values_0_65x85.tiff",
////            "test_data/image_processing_pipeline/subtract/test_image_all_values_313_65x85.tiff",
////            "test_data/image_processing_pipeline/subtract/test_image_all_values_353_65x85.tiff",
////            "test_data/image_processing_pipeline/subtract/test_image_all_values_541_65x85.tiff"
////    };
////
////    std::shared_ptr<ImageT> averageImg = average(imageNames | images());
////
////    DEBUG_IMAGE_DISPLAY(*averageImg,"after-average", 1);
////}
//
///**
// * NOTE: A Data-testcase was not possible here because having a nested container inside the bdata spec.
// *       didn't compile.
// */
////BOOST_AUTO_TEST_CASE(pipeline_files_test)
////{
////    using namespace AstroImagePipeline;
////
////    std::vector<std::string> expectedTiffFilenames = { "test_data/image_processing_pipeline/files/tiff_file_2.tiff",
////                                                     "test_data/image_processing_pipeline/files/tiff_file_1.tiff" };
////
////    BOOST_TEST(files(fs::path("test_data/image_processing_pipeline/files"), ".tiff") == expectedTiffFilenames);
////
////
////    std::vector<std::string> expectedFitsFilenames = { "test_data/image_processing_pipeline/files/fits_file_1.fits",
////                                                     "test_data/image_processing_pipeline/files/fits file with spaces 2.fits",
////                                                     "test_data/image_processing_pipeline/files/fits file with spaces 1.fits",
////                                                     "test_data/image_processing_pipeline/files/fits_file_2.fits" };
////
////    BOOST_TEST(files(fs::path("test_data/image_processing_pipeline/files"), ".fits") == expectedFitsFilenames);
////
////
////    std::vector<std::string> expectedXyzFilenames = { };
////
////    BOOST_TEST(files(fs::path("test_data/image_processing_pipeline/files"), ".xyz") == expectedXyzFilenames);
////
////
////    std::vector<std::string> expectedNoExtFilenames = {
////            "test_data/image_processing_pipeline/files/tiff_file_2.tiff",
////            "test_data/image_processing_pipeline/files/fits_file_1.fits",
////            "test_data/image_processing_pipeline/files/fits file with spaces 2.fits",
////            "test_data/image_processing_pipeline/files/txt_file_2.txt",
////            "test_data/image_processing_pipeline/files/txt_file_1.txt",
////            "test_data/image_processing_pipeline/files/fits file with spaces 1.fits",
////            "test_data/image_processing_pipeline/files/noext_file_2",
////            "test_data/image_processing_pipeline/files/tiff_file_1.tiff",
////            "test_data/image_processing_pipeline/files/fits_file_2.fits",
////            "test_data/image_processing_pipeline/files/noext_file_1"
////    };
////
////    BOOST_TEST(files(fs::path("test_data/image_processing_pipeline/files")) == expectedNoExtFilenames);
////}
//
//
////// TODO: Refactor..
////BOOST_AUTO_TEST_CASE(pipeline_test_1)
////{
////        using namespace boost::range;
////        using namespace boost::adaptors;
////        using namespace AstroImagePipeline;
////
////        // 1.
////        //        images("test_data/image_processing_pipeline/image_set_1/*.tiff") OR imageFileNames
////        //            | read()
////        //            | filtered(! StarAnalysisT::isSaturated())
////        //            | filtered(StarAnalysisT::isSingleStar())
////        //            | subtractBackground(ThresholderT::otsu(...) OR thresholder function)
////        //            | scaleUp(10.0F, scale-method)
////        //            | centerOnStar(CentroiderT::iwc(...), image size)
////        //            | scaleDown(10.0F, scale-method)
////        //
////        //            // See https://www.caichinger.com/boost-range/boost-accumulate.html
////        //            //int sum = boost::accumulate(vec, 0);
////
////
////    // TODO: Try the same thing with C++20... views...
////
//////    const std::vector<std::string> inputStrings = {"aaa", "bbb" };
//////
//////    for (auto result :
//////            flatten(inputStrings | produce_range_of_ranges())) {
//////        std::cerr << "result: " << result << std::endl;
//////    }
////
//////        std::vector persons{Person{}, Person{}, Person{}};
////
//////        | std::views::transform([](auto & p) {
//////            std::vector<std::string> v;
//////            v.emplace_back("aaa");
//////            v.emplace_back("bbb");
//////            v.emplace_back("ccc");
//////            return v;
//////        })
////
////        std::vector<std::string> searchPaths = {"test_data/image_processing_pipeline/files/"};
////
////        for (const auto& ch : searchPaths
////                              | files("(.*\\.fits)")
////                              | std::views::join
////                              )
////        {
////            std::cerr << ch << std::endl;
////        }
////
//////        for (const auto& ch : persons
//////                              | std::views::transform(&Person::name)
//////                              | std::views::join)
//////        {
//////            std::cerr << ch << std::endl;
//////        }
////
////
//////    for(auto result :
//////            files("test_data/image_processing_pipeline/image_set_1", ".tiff") | images()) {
////
//////        -> NOTE: Return value in this case is IGNORED! It is not automatically forwarded to the pipe!
//////        Das ist ein Denkfehler! Idee: Konkrete aufruf-Reihenflolge in operator|() syntax hinschreiben!
////       // TODO: This crashes... somehow moving a local vector does not work...
//////    }
////
//////    for(auto result :
//////             files("test_data/image_processing_pipeline/image_set_1", ".tiff")
//////                | images() // TODO: Maybe rename to load_image()
//////                | filtered(&StarAnalysis::isNotSaturated)
//////                | subtract_background(ThresholdingAlgorithmFactoryT::getInstance(ThresholdingAlgorithmTypeT::OTSU))
//////                | scale_up(3.0F)
//////                | center_on_star(CentroidAlgorithmFactoryT::getInstance(CentroidAlgorithmTypeT::IWC))
//////                | scale_down(3.0F)
//////                | crop_from_center(SizeT<int>(61,61))
////////                | crop(rect) OR
//////        ) {
//////        HfdT hfd(*result);
//////        std::cerr << "HFD: " << hfd.getValue() << std::endl;
//////    }
////
////
////
////// 2.
/////**
//// *         AstroImageProcessingPipelineT
//// *              .of("light-frames/*.fits")                                                  -> List<img>
//// *              .subtract(ImageProcessingPipelineT.of("dark-frames/*.fits").average())      -> List<img>
//// *              .divide(ImageProcessingPipelineT.of("flatfield-frames/*.fits").average())   -> List<img>
//// *              .average()                                                                  -> img
//// *              .stretch(StretcherT::-...)                                                  -> img
//// *              .store/save("my-filename.png")
//// */
////
////
//// TODO: average() may be changed to | average() using actions of ranges-v3!
//////    std::shared_ptr<ImageT> averageImg = average(imageNames | images());
//////
//////    for(auto result : imageNames
//////                | images()
//////                | subtract(img...)
//////                | value_clip(ClippingAlgorithmT)
////
////
//////    BOOST_CHECK(resultImages.size() == 11);
////}
//
//BOOST_AUTO_TEST_SUITE_END();
