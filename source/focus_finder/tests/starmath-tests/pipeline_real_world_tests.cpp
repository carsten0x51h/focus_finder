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

#include <iostream>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/action/join.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/core.hpp>   // ranges::front()
#include <range/v3/algorithm/for_each.hpp>

#include <boost/test/unit_test.hpp>

#include "../../common/include/pipeline/view/files.h"
#include "../../common/include/pipeline/view/read.h"
#include "../../common/include/pipeline/view/write.h"
#include "../../common/include/pipeline/view/crop.h"
#include "../../common/include/pipeline/view/scale.h"
#include "../../common/include/pipeline/view/subtract.h"
#include "../../common/include/pipeline/view/divide_by.h"
#include "../../common/include/pipeline/view/subtract_background.h"
#include "../../common/include/pipeline/view/center_on_star.h"
#include "../../common/include/pipeline/view/interpolate_bad_pixels.h"

#include "../../common/include/pipeline/action/average.h"

#include "../../common/include/thresholding_algorithm_factory.h"
#include "../../common/include/centroid_algorithm_factory.h"
#include "../../common/include/star_analysis.h"

#include "../../common/include/snr.h"
#include "../../common/include/hfd.h"
#include "../../common/include/fwhm.h"
#include "../../common/include/image_slicer.h"
#include "../../common/include/floating_point_equality.h"
#include "../../common/include/pipeline/view/detect_stars.h"
#include "../../common/include/pipeline/view/remove_nans.h"


BOOST_AUTO_TEST_SUITE(pipeline_combination_tests)

using namespace starmath;
using namespace starmath::pipeline;
using namespace starmath::pipeline::io;
using namespace ranges;


/**
 *
 */
// TODO: Implement simple crop().
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
            | read()
            | view::filter(& metrics::is_not_saturated)
            | subtract_background(ThresholdingAlgorithmFactoryT::getInstance(ThresholdingAlgorithmTypeT::OTSU)) // TODO: Why not just passing in ThresholdingAlgorithmTypeT::OTSU?
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
 * IDEA:
 *         AstroImageProcessingPipelineT
 *              .of("light-frames/*.fits")                                                  -> List<img>
 *              .subtract(ImageProcessingPipelineT.of("dark-frames/*.fits").average())      -> List<img>
 *              .divide(ImageProcessingPipelineT.of("flatfield-frames/*.fits").average())   -> List<img>
 *              .average()                                                                  -> img
 *              .stretch(StretcherT::-...)                                                  -> img
 *              .store/save("my-filename.png")
 *
 * NOTE: This does no alignment!
 *
 * TODO: Add     | value_clip(ClippingAlgorithmT) after subtract?
 */
BOOST_AUTO_TEST_CASE(pipeline_astrophotography_image_development_test, * boost::unit_test::tolerance(0.1))
{
    const std::string base_path = "test_data/image_processing_pipeline/real_world/astrophotography/image_development/";

    auto dark_files = view::single(base_path + "dark")
                              | files("(.*\\.fit\\.gz)") | view::join | to<std::vector>();

    auto dark_flat_files = view::single(base_path + "dark_flat")
                              | files("(.*\\.fit\\.gz)") | view::join | to<std::vector>();

    auto flat_files = view::single(base_path + "flat")
                              | files("(.*\\.fit\\.gz)") | view::join | to<std::vector>();

    auto light_frame_files = view::single(base_path + "light")
                              | files("(.*\\.fit\\.gz)") | view::join | to<std::vector>();

    // NOTE: If just one pixel has a NAN value (e.g. div by 0), BOOST_TEST() fails when comparing two
    // images, even if both images have a "NAN value" at the same pixel position. The reason is that
    // NAN == NAN is false. Therefore, remove_nans() is the last step which uses a median blur filter
    // to interpolate all NAN values using their surrounding neighbours.
    //
    // TODO: Should fail if e.g. dimension of dark files is different from dimension of light frames!
    auto light_average_no_nans_range =
             view::single(
	      average(
                light_frame_files
                    | read()
				    | interpolate_bad_pixels(500 /*threshold*/, 3 /*filter size*/)
					| subtract(
                        average(dark_files
                        		| read()
								| interpolate_bad_pixels(500 /*threshold*/, 3 /*filter size*/)
                        )
                      )
                    | divide_by(
                        average(
                            flat_files
                                | read()
							    | interpolate_bad_pixels(500 /*threshold*/, 3 /*filter size*/)
								| subtract(
                                     average(dark_flat_files
                                    		 | read()
											 | interpolate_bad_pixels(500 /*threshold*/, 3 /*filter size*/)
									 )
                                  )
                        )
                     )
	      )
	     );
//             | remove_nans();

    

    // This line initially generated the expected result.
    std::string expected_image_filename = base_path + "expected_result.tiff";
    ImageT expected_result(expected_image_filename.c_str());

    // Just one image is expected as result from the processing pipeline
    BOOST_TEST(size(light_average_no_nans_range) == 1);

    // The ranges::front() call extracts the only image from the range (here a std::shared_ptr<ImageT>). 
    auto calculated_img = *ranges::front(light_average_no_nans_range);
	
    //calculated_img.save(expected_image_filename.c_str()); // HACK: Only to produce result, once

    BOOST_TEST(isAlmostEqual(calculated_img, expected_result, 0.00001));
}


/**
 *      3. Cluster example (old automatic star analyzer)
 *
 *          List<img> imgs = AstroImageProcessingPipelineT
 *                              .of("my-star-image.fits")
 *                              .denoise(DenoiserT::...)
 *                              .subtractBackground(ThresholderT::otsu(...) OR thresholder function)
 *                              .cluster(ClusterAlgorithmT::...)                                          -> List<List<img>>
 *                              .flatMap()
 *                              .filter(StarDetailsT::isSaturated() == false)
 *                              .centerOnStar(CentroiderT::iwc(...))
 *                              .filter(StarDetailsT::hfd() < 2)   ??
 *                              .filter(StarDetailsT::snr() > 10)  ??
 *                              .collect(Collector::asList())
 *
 *          OR with range library
 *
 *          NOTE: flatmap: https://stackoverflow.com/questions/36051851/how-to-implement-flatmap-using-rangev3-ranges
 *
 *          images("my-star-image.fits")
 *              | denoise(DenoiserT::...())
 *              | subtractBackground(ThresholderT::otsu(...) OR thresholder function)
 *              | cluster(ClusterAlgorithmT::...)
 *              | boost::range::for_each() -> Range of images (NOTE: Images can have different sizes)
 *              | filtered(! StarAnalysisT::isSaturated())
 *              | centerOnStar(CentroiderT::iwc(...))
 *              | filtered(! StarAnalysisT::snr() > 10)
 *              | filtered(! StarAnalysisT::hfd() < 5)
 *
 * TODO: Why is pipeline processed twice? -> detet_stars() ... to<vector>()?...
 * TODO: Smaller input images with only 2-3 stars... but multiple ones...
 * TODO: Add hot-pixel removal before detection,,,, maybe also denoise()...
 * TODO / IDEA: Add example-program which generates an image where the detected stars are marked ... + HFD - like the original star-recognizer...
 * TODO: Add center_on_star() for each detected star image... then calc SnrT, HfdT and FwhmT....
 */
BOOST_AUTO_TEST_CASE(pipeline_star_recognizer_test)
{
    auto clusteredImagesRanges =
            view::single("test_data/image_processing_pipeline/real_world/star_recognizer/test_image_star_recognizer_1.fit.gz")
              | read()
              | interpolate_bad_pixels(500 /*threshold*/, 3 /*filter size*/)
			  | detect_stars(2 /*cluster radius*/,
					        ThresholdingAlgorithmFactoryT::getInstance(ThresholdingAlgorithmTypeT::OTSU),
							30
				)
              | crop()
              | views::join
	          | scale_up(3.0F)
	          | center_on_star(CentroidAlgorithmFactoryT::getInstance(CentroidAlgorithmTypeT::IWC))
	          | scale_down(3.0F)
			  | write("img_%4d.fit")
			  | to<std::vector>();



//    BOOST_TEST(clusteredImagesRanges.size() == 1); // Should correspond to the number of input images
    BOOST_TEST(clusteredImagesRanges.size() == 92); // 216 detected stars (without hot-pixel removal)


    // Test; Calculate SNR and HFD
    // NOTE: Question is, if before HFD calculation, the background should be subtracted, or not....
    //       and if this should happen inside the HFD... No, because the HFD algorithm is
    //       independent from that. It is correct for both images - with and without background.
    //       Just the value it returns, is quite different.
//    auto starMetricsRange =
//    		clusteredImagesRanges
//	        | subtract_background(ThresholdingAlgorithmFactoryT::getInstance(ThresholdingAlgorithmTypeT::OTSU)) // TODO: Why not just passing in ThresholdingAlgorithmTypeT::OTSU?              | scale_up(3.0F)
//			| view::transform(
//					[](const auto & imgPtr) {
//            			return std::make_tuple(SnrT::calculate(*imgPtr), HfdT(*imgPtr).getValue());
//    				});


//    ranges::for_each(starMetricsRange, [](const auto & starMetrics){
//        std::cout << "SNR: " << std::get<0>(starMetrics)
//        		<< ", HFD: " << std::get<1>(starMetrics)
//				<< std::endl;
//    });


    // DEBUG START
    // Write result images...
//    int counter = 0;
//
//    for(const auto & starImg : clusteredImagesRanges) {
//    	std::stringstream ss;
//    	ss << "star_img" << counter++ << ".png";
//    	std::cerr << "Storing " << ss.str() << "..." << std::endl;
//
//    	starImg->save(ss.str().c_str());
//    }
    // DEBUG END
}

BOOST_AUTO_TEST_SUITE_END();
