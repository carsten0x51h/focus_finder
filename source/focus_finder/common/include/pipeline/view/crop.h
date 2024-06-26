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

#ifndef STARMATH_PIPELINE_CROP_H_
#define STARMATH_PIPELINE_CROP_H_ STARMATH_PIPELINE_CROP_H_

#include <range/v3/view/transform.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/join.hpp>


#include "../../logging.h"
#include "../../image.h"
#include "../../point.h"
#include "../../rect.h"
#include "../../size.h"

#define STARMATH_CROP_DEBUG 0

namespace starmath::pipeline {

  /**
   *
   */
  template<typename CropRegionRngT, typename ImageType=float>
  static auto
  //static std::vector<std::shared_ptr<cimg_library::CImg<ImageType>>>
  cropInternal(const CropRegionRngT crop_regions, const std::shared_ptr<cimg_library::CImg<ImageType>> &image) {

	 std::cerr << "Inside cropInternal!..." << std::endl;
	 std::cerr << "Calling size: " << ranges::size(crop_regions) << std::endl;

	 auto vec = crop_regions | ranges::to<std::vector>();
	 std::cerr << "Calling size vec: " << vec.size()<< std::endl;


	return vec
	  | ranges::view::transform(
					[=](const auto & crop_region) {

					  std::cerr << "Cropping region ..." << std::endl;
					  //std::cerr << "Cropping region " << crop_region << std::endl;

					  // See https://github.com/GreycLab/CImg/issues/110
//					  return std::make_shared<cimg_library::CImg<ImageType>>(
//														image->get_crop(
//															crop_region.x() /*x0*/,
//															crop_region.y() /*y0*/,
//															crop_region.x() + crop_region.width() - 1/*x1*/,
//															crop_region.y() + crop_region.height() - 1/*y1*/
//														)
//													);
					  return std::make_shared<cimg_library::CImg<ImageType>>();
					}
				);
//	  | ranges::to<std::vector>();
  }


  template<typename ImageType=float>
  auto
  //std::vector<std::shared_ptr<cimg_library::CImg<ImageType>>>
  crop() {
	  return ranges::view::transform(
	  					  [=](const auto & imageRectsPair) {
	      				auto img = imageRectsPair->first;
	      				//const std::vector<RectT<int>> & cropRects = imageRectsPair->second;

//	      				return cropInternal(cropRects, img); // std::vector<std::shared_ptr<cimg_library::CImg<ImageType>>>


	      				return imageRectsPair->second
	    	      		   | ranges::view::transform(
	      								[=](const auto & crop_region) {

	      								  LOG(debug) << "Cropping region " << crop_region << std::endl;

	      								  // See https://github.com/GreycLab/CImg/issues/110
	      								  return std::make_shared<cimg_library::CImg<ImageType>>(
	      										img->get_crop(
	      																		crop_region.x() /*x0*/,
	      																		crop_region.y() /*y0*/,
	      																		crop_region.x() + crop_region.width() - 1/*x1*/,
	      																		crop_region.y() + crop_region.height() - 1/*y1*/
	      																	)
	      																);
	      								}
	      							) | ranges::to<std::vector>();




	  			  });
  }
  
  
  template<typename CropRegionRngT, typename ImageType=float>
  auto
  crop(const CropRegionRngT & crop_regions) {
	return ranges::view::transform(
								   [=](const std::shared_ptr<cimg_library::CImg<ImageType>> &image) {
									 return cropInternal(crop_regions, image);
								   });
  }


  template<typename ImageType=float>
  auto
  crop(const RectT<int> &crop_region) {
	return ranges::view::transform(
								   [=](const std::shared_ptr<cimg_library::CImg<ImageType> > &image) {
									 	 return cropInternal(ranges::view::single(crop_region), image);
								   });
  }
  
  
  template<typename ImageType=float>
  auto
  crop_from_center(const SizeT<int> &crop_region) {
	return ranges::view::transform(
								   [=](const std::shared_ptr<cimg_library::CImg<ImageType> > &image) {

									 const cimg_library::CImg<ImageType> &inputImageRef = *image;

									 DEBUG_IMAGE_DISPLAY(inputImageRef, "crop_from_center_in", STARMATH_CROP_DEBUG);

									 // TODO: Maybe this calculation can be simplified...
									 PointT<float> imageCenter((float) inputImageRef.width() / 2.0F,
															   (float) inputImageRef.height() / 2.0F);

									 RectT<float> rect = RectT<float>::fromCenterPoint(imageCenter, crop_region.to<float>());


									 // get_crop() by default applies a dirichlet boundary_condition (=0). There are other
									 // options as well. In this case, the desired behaviour is to assume that all pixel values
									 // where the defined sub-frame exceeds the image boundary are assumed to be 0.
									 //
									 // boundary_conditions	= Can be { 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }
									 //
									 // - Dirichlet means "0 outside image".
									 // - Neumann means "Nearest neighbor outside image" (i.e. null derivative)
									 // - Periodic means "Periodic"
									 // - Mirror means "Mirrored image outside".
									 //
									 // See https://github.com/GreycLab/CImg/issues/110
									 auto croppedImg = std::make_shared<cimg_library::CImg<ImageType>>(
																		inputImageRef.get_crop(
																		        rect.x() /*x0*/, rect.y() /*y0*/,
																				rect.x() + rect.width() - 1/*x1*/,
																				rect.y() + rect.height() - 1/*y1*/)
																		);

									 DEBUG_IMAGE_DISPLAY(*croppedImg, "crop_from_center_out", STARMATH_CROP_DEBUG);

									 return croppedImg;
								   }
								   );
  }
}

#endif //STARMATH_PIPELINE_CROP_H_
