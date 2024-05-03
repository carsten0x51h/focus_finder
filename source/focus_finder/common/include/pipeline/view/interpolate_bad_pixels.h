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
#include "../../bad_pixel_median_interpolator.h"

#define STARMATH_INTERPOLATE_BAD_PIXELS_DEBUG 1

namespace starmath::pipeline {
  
  
    /**
     * If the potential "bad" pixel value deviates more than this "factor" from the mean
	 * of the surrounding pixels, then the pixel is removed, i.e. interpolated by the mean
	 * value of the surrounding pixels.
	 *
     * @param ImageType
	 * @param Filter core size (N x N), odd number > 1, allowed values are 3,5,7,9
     * @param absoluteDetectionThreshold
	 *
     * @return Image with interpolated bad pixels (shared_ptr<cimg_library::CImg<ImageType>>)
	 *
	 *
	 * TODO: Move the actual algorithm out of this pipeline function to the "algorithms" and also write
	 *       unit tests for this filter independent from this pipeline functionality.
	 *
	 * TODO: Add:
	 *      - relativeDetectionThreshold
     */
    template<typename ImageType=float>
    auto
	interpolate_bad_pixels(float absoluteDetectionThreshold = 500, unsigned int filterCoreSize = 3, BadPixelMedianInterpolatorT::ThresholdDirectionT::TypeE thresholdDirection = BadPixelMedianInterpolatorT::ThresholdDirectionT::BOTH) {
	  return ranges::views::transform(
            [=](const std::shared_ptr<cimg_library::CImg<ImageType> > &image) {
                const cimg_library::CImg<ImageType> &inputImageRef = *image;

                DEBUG_IMAGE_DISPLAY(inputImageRef, "interpolate_bad_pixels_in", STARMATH_INTERPOLATE_BAD_PIXELS_DEBUG);

				BadPixelMedianInterpolatorT badPixelMedianInterpolator(absoluteDetectionThreshold, filterCoreSize, thresholdDirection);

				auto result_image = badPixelMedianInterpolator.interpolate(inputImageRef);
				
                DEBUG_IMAGE_DISPLAY(*result_image, "interpolate_bad_pixels_out", STARMATH_INTERPOLATE_BAD_PIXELS_DEBUG);

                return result_image;
            }
        );
    }

}

#endif // STARMATH_INTERPOLATE_BAD_PIXELS_H
