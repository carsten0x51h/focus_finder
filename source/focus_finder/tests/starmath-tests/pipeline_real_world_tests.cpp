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
#include <range/v3/view/join.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/action/sort.hpp>

#include <boost/test/unit_test.hpp>

#include "../../common/include/pipeline/view/files.h"
#include "../../common/include/pipeline/view/images.h"
#include "../../common/include/pipeline/view/crop.h"
#include "../../common/include/pipeline/view/scale.h"
#include "../../common/include/pipeline/view/subtract_background.h"
#include "../../common/include/pipeline/view/center_on_star.h"


#include "../../common/include/thresholding_algorithm_factory.h"
#include "../../common/include/centroid_algorithm_factory.h"
#include "../../common/include/star_analysis.h"

#include "../../common/include/snr.h"
#include "../../common/include/hfd.h"
#include "../../common/include/fwhm.h"
#include "../../common/include/image_slicer.h"


BOOST_AUTO_TEST_SUITE(pipeline_combination_tests)

using namespace starmath::pipeline;
using namespace starmath;
using namespace ranges;


/**
 *
 */
//    std::vector<std::string> filePaths {
//      "test_data/image_processing_pipeline/real_world/star_metrics"
//    };
//    | files("(.*\\.tiff)")
//      | view::join
// TODO / IDEA: Move "own" views to namespace "view" - a sub-namespace to "starmath".
//              -> Then starmath::view::center_on_star().... can be written - or, if
//                 "using namespace starmath" is used, simply view::center_on_star().
//// TODO: Implement simple crop().
// TODO / PROBLEM: This way we lose the info which image we talk about... -> include path into ImageT structure...?!
//                 -> This would also be helpful with a "store()/save()" function.
//
// TODO: Move HfdT to starmath::metrics::Hfd
// TODO: To be changed to HfdT::calculate(...)? Or does the objet hold some valuable additional infos - like getOuterRadius()... which will now depend on the image input!
// FwhmT fwhmObj(ImageSlicerT::slice<SliceDirectionT::VERT>(*result, result->width()/2)); // TODO: Change: pass in entire image -> calculate mean of VERT + HORZ Fwhm and return this as value. FwhmT should provide functions to get both separately.
//
BOOST_AUTO_TEST_CASE(pipeline_star_metrics_test, * boost::unit_test::tolerance(0.1))
{
    std::vector<std::string> imagePaths {
        "test_data/image_processing_pipeline/real_world/star_metrics/newton_focus_star1.tiff",
        "test_data/image_processing_pipeline/real_world/star_metrics/newton_focus_star2.tiff",
        "test_data/image_processing_pipeline/real_world/star_metrics/newton_focus_star3.tiff",
        "test_data/image_processing_pipeline/real_world/star_metrics/newton_focus_star4.tiff",
        "test_data/image_processing_pipeline/real_world/star_metrics/newton_focus_star5.tiff",
        "test_data/image_processing_pipeline/real_world/star_metrics/newton_focus_star6.tiff",
        "test_data/image_processing_pipeline/real_world/star_metrics/newton_focus_star7.tiff",
        "test_data/image_processing_pipeline/real_world/star_metrics/newton_focus_star8.tiff",
        "test_data/image_processing_pipeline/real_world/star_metrics/newton_focus_star9.tiff",
        "test_data/image_processing_pipeline/real_world/star_metrics/newton_focus_star10.tiff",
        "test_data/image_processing_pipeline/real_world/star_metrics/newton_focus_star11.tiff"
    };

    // TODO: Interesting observation: SNR gets worse again whens tar gets more into focus. Why? Is that expected?
    std::vector<std::tuple<double, double>> expected_star_metrics = {
        { 3.4248103734966566, 27.295144884427497 },
        { 4.0168380799027004, 25.372589848114831 },
        { 4.2398086088788904, 23.906962737212321 },
        { 4.5106426505897472, 21.773008162322135 },
        { 5.9366687230635415, 18.646887642682195 },
        { 5.9214817393646637, 15.807635535535937 },
        { 7.2058655558692744, 12.081075787803762 },
        { 6.7269579863995439, 9.182079739644923 },
        { 7.4242741594713344, 6.2974607834559153 },
        { 5.278791273812149, 3.5406091827173412 },
        { 4.1407320775829746, 2.7410637875414858 }
    };

    auto star_metrics =
        imagePaths
            | images()
            | view::filter(& metrics::is_not_saturated) // TODO: Better name for StarAnalysis? filter::? metrics?
            | subtract_background(ThresholdingAlgorithmFactoryT::getInstance(ThresholdingAlgorithmTypeT::OTSU))
            | scale_up(3.0F)
            | center_on_star(CentroidAlgorithmFactoryT::getInstance(CentroidAlgorithmTypeT::IWC))
            | scale_down(3.0F)
            | crop_from_center(SizeT<int>(61,61))
            | view::transform(
                [](const auto & imgPtr){
                    return std::make_tuple(SnrT::calculate(*imgPtr), HfdT(*imgPtr).getValue());
                })
            | to<std::vector>();

    BOOST_TEST(star_metrics == expected_star_metrics);
}


/**
 *
 */
BOOST_AUTO_TEST_CASE(pipeline_astrophotography_image_development_test)
{
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
}


/**
 *
 */
BOOST_AUTO_TEST_CASE(pipeline_star_recognizer_test)
{
}

BOOST_AUTO_TEST_SUITE_END();
