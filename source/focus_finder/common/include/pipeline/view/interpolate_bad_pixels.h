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

#ifndef STARMATH_INTERPOLATE_BAD_PIXELS_H
#define STARMATH_INTERPOLATE_BAD_PIXELS_H STARMATH_INTERPOLATE_BAD_PIXELS_H

#include <range/v3/view/transform.hpp>

#include "../../image.h"

#define STARMATH_INTERPOLATE_BAD_PIXELS_DEBUG 1

namespace starmath::pipeline {

    /**
     * If the potential "bad" pixel value deviates more than this "factor" from the mean
	 * of the surrounding pixels, then the pixel is removed, i.e. interpolated by the mean
	 * value of the surrounding pixels.
	 *
     * @param ImageType
	 * @param Filter core size (N x N), odd number > 1
     * @param absoluteDetectionThreshold
	 *
	 *		  From the CImg documentation it is not clear if the "threshold" which can be passed
	 *		  optionally to the blur_median function works in both drections. It is also not clear
	 *		  if it is a relative or an absolute value with respect to the mean of the surrounding
	 *		  pixels. However, the code shows the following condition:
	 *
	 *        if (cimg::abs((*this)(p,q,c) - val0)<=threshold) ...
	 *
	 *        val0 is the pixel value That means, it is an absolute value which works in both directions. That means, the
	 *        function can only be used in that particular case.
	 *
     * @return Image with interpolated bad pixels (shared_ptr<cimg_library::CImg<ImageType>>)
	 *
	 *
	 * TODO: Check that filterCoreSize is odd and > 1...
	 * TODO: Move the actual algorithm out of this pipeline function to the "algorithms" and also write
	 *       unit tests for this filter independent from this pipeline functionality.
	 *
	 * TODO: Later, add:
	 *      - relativeDetectionThreshold
	 *      - direction: POSITIVE (hot pixels), NEGATIVE (cold pixels), BOTH (hot- and cold pixels)
     */
    template<typename ImageType=float>
    auto
	  interpolate_bad_pixels(float absoluteDetectionThreshold = 500, unsigned int filterCoreSize = 3) {
        return ranges::views::transform(
            [=](const std::shared_ptr<cimg_library::CImg<ImageType> > &image) {
                const cimg_library::CImg<ImageType> &inputImageRef = *image;

                DEBUG_IMAGE_DISPLAY(inputImageRef, "interpolate_bad_pixels_in", STARMATH_INTERPOLATE_BAD_PIXELS_DEBUG);

                auto result_image = std::make_shared<cimg_library::CImg<ImageType>>(inputImageRef);
				using namespace cimg_library;
				// See https://cimg.eu/reference/loops_Using.html
				//CImg_3x3(I,ImageType);
				CImg<ImageType> I(3,3);  
				cimg_for3x3(inputImageRef,x,y,0,0,I,ImageType) {
				  //T sum = Ipp + Icp + Inp + Ipc + Inc + Ipn + Icn + Inn;
				  //T mean = sum / 8.0F;
				  //if (std::abs(Icc - mean) > absoluteDetectionThreshold) {
				  //}
				  ImageType med = I.median();
				  
				  if (std::abs(inputImageRef(x,y) - med) > absoluteDetectionThreshold) {
					(*result_image)(x,y) = med;
				  }
				}

				// Size of the border which will not be looped over by the next command
				// is half of the filter core size. This is to avoid the filter core
				// accessing pixels outside the image.
				//const unsigned int border = std::ceil((float)filterCoreSize / 2.0F);
				
				// See https://cimg.eu/reference/loops_Using.html
				// Loop along the (x,y)-axes, excepted for pixels inside a border of n pixels wide.
				/* cimg_for_insideXY(result_image,x,y,border) { */
				  
				/* } */
				
                /* auto result_image = */
				/*   std::make_shared<cimg_library::CImg<ImageType>>( */
				/* 												  inputImageRef.get_blur_median (filterCoreSize, absoluteDetectionThreshold) */
				/* 												  ); */
				
                DEBUG_IMAGE_DISPLAY(*result_image, "interpolate_bad_pixels_out", STARMATH_INTERPOLATE_BAD_PIXELS_DEBUG);

                return result_image;
            }
        );
    }

}

#endif // STARMATH_INTERPOLATE_BAD_PIXELS_H
