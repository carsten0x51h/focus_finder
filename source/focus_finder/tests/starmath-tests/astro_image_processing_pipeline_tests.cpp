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

#include <vector>
//#include <ranges>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>

//#include <boost/range/adaptor/transformed.hpp>
//#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
//#include <boost/range/any_range.hpp>
//
//#include <boost/range/adaptor/argument_fwd.hpp>
//#include <boost/range/iterator_range.hpp>
//#include <boost/iterator/transform_iterator.hpp>
//#include <boost/range/numeric.hpp>

#include "../../common/include/image.h"
#include "../../common/include/thresholding_algorithm_factory.h"
#include "../../common/include/pipeline/adapter/images.h"
#include "../../common/include/pipeline/adapter/subtract_background.h"


namespace bdata = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(astro_image_processing_pipeline_tests)



BOOST_AUTO_TEST_CASE(astro_image_processing_pipeline_test_1)
{
    using namespace boost::range;
    using namespace boost::adaptors;

    std::vector<std::string> imageNames = {
            "test_data/image_processing_pipeline/image_set_1/newton_focus_star1.tiff",
            "test_data/image_processing_pipeline/image_set_1/newton_focus_star2.tiff",
            "test_data/image_processing_pipeline/image_set_1/newton_focus_star3.tiff",
            "test_data/image_processing_pipeline/image_set_1/newton_focus_star4.tiff",
            "test_data/image_processing_pipeline/image_set_1/newton_focus_star5.tiff",
            "test_data/image_processing_pipeline/image_set_1/newton_focus_star6.tiff",
            "test_data/image_processing_pipeline/image_set_1/newton_focus_star7.tiff",
            "test_data/image_processing_pipeline/image_set_1/newton_focus_star8.tiff",
            "test_data/image_processing_pipeline/image_set_1/newton_focus_star9.tiff",
            "test_data/image_processing_pipeline/image_set_1/newton_focus_star10.tiff",
            "test_data/image_processing_pipeline/image_set_1/newton_focus_star11.tiff"
    };

    using namespace AstroImagePipeline;

    std::vector<ImageT> res;

    copy(
            imageNames
                | images(123, 456)
                | subtract_background(ThresholdingAlgorithmTypeT::OTSU),
            std::back_inserter(res)
    );

    for(auto & s : res) {
        std::cerr << "s: " << s.height() << std::endl;
    }

//        images("test_data/image_processing_pipeline/image_set_1/*.tiff") OR imageFileNames
//            | read()
//            | filtered(! StarAnalysisT::isSaturated())
//            | filtered(StarAnalysisT::isSingleStar())
//            | subtractBackground(ThresholderT::otsu(...) OR thresholder function)
//            | scaleUp(10.0F, scale-method)
//            | centerOnStar(CentroiderT::iwc(...), image size)
//            | scaleDown(10.0F, scale-method)
//
//
//            // See https://www.caichinger.com/boost-range/boost-accumulate.html
//            //int sum = boost::accumulate(vec, 0);


//    // vs
//    //ImagePipelineFunctionsT::load(imageNames)
//
////    PipelineBuilderT::of(imageNames)
////                        .scaleUp(2.0F)
////                        .collect();
//
//
//    auto loadImage = [](const std::string & imageName) -> ImageT { return ImageT(imageName.c_str()); };
//    auto scaleUp = [](cimg_library::CImg<float> img) { return img.get_resize(img.width() * 2, img.height() * 2); };
//
//    std::list<ImageT> resultImages;
//
//    float scaleFactor = 2.0F;
//
////    template< class SinglePassRange, class UnaryFunction >
////    transformed_range<UnaryFunction,SinglePassRange> x = transformed(scaleUp);
//
//    copy(
//        imageNames |
//        transformed(loadImage) |
//        transformed([=](const ImageT & img) { return img.get_resize(img.width() * scaleFactor, img.height() * scaleFactor); }) |
//        filtered(
//            [](const ImageT & img) { return (img.width() > 2); }
//        ),
//        std::back_inserter(resultImages));
//
//    std::cerr << "Size: " << resultImages.size() << std::endl;
//
//    BOOST_CHECK(resultImages.size() == 11);
}

BOOST_AUTO_TEST_SUITE_END();
