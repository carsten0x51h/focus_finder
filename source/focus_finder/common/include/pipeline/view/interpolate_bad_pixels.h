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
   * TODO: Should this be here?
   */
  struct ThresholdDirectionT {
    enum TypeE {
	  POSITIVE, // Hot pixels
	  NEGATIVE, // Cold pixels
	  BOTH,     // Hot- and cold pixels
	  _Count
    };

    static const char *asStr(const TypeE &inType) {
	  switch (inType) {
	  case POSITIVE:
		return "POSITIVE";
	  case NEGATIVE:
		return "NEGATIVE";
	  case BOTH:
		return "BOTH";
	  default:
		return "<?>";
	  }
    }

    MAC_AS_TYPE(Type, E, _Count);
  };

  
  
  /**
   * TODO: Move to .cpp file? Or solve it via a #define?
   */
  template<typename ImageType>
  ImageType interpolateInternal(ImageType pixelValue, const cimg_library::CImg<ImageType> & neighbourhood, float threshold, ThresholdDirectionT::TypeE thresholdDirection) {
	ImageType med = neighbourhood.median();

	bool wantInterpolation = false;
	
	switch(thresholdDirection) {
	case ThresholdDirectionT::POSITIVE:
	  wantInterpolation = ((pixelValue - med) > threshold);
	  break;
	  
	case ThresholdDirectionT::NEGATIVE:
	  wantInterpolation = ((med - pixelValue) > threshold);
	  break;
	  
	case ThresholdDirectionT::BOTH:
	  wantInterpolation = (std::abs(pixelValue - med) > threshold);
	  break;
	  
	  //default:
	  // TODO: throw...
	}
	return (wantInterpolation ? med : pixelValue);
  }
  
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
	interpolate_bad_pixels(float absoluteDetectionThreshold = 500, unsigned int filterCoreSize = 3, ThresholdDirectionT::TypeE thresholdDirection = ThresholdDirectionT::BOTH) {
	  return ranges::views::transform(
            [=](const std::shared_ptr<cimg_library::CImg<ImageType> > &image) {
                const cimg_library::CImg<ImageType> &inputImageRef = *image;

                DEBUG_IMAGE_DISPLAY(inputImageRef, "interpolate_bad_pixels_in", STARMATH_INTERPOLATE_BAD_PIXELS_DEBUG);

                auto result_image = std::make_shared<cimg_library::CImg<ImageType>>(inputImageRef);

				// See https://cimg.eu/reference/loops_Using.html
				cimg_library::CImg<ImageType> neighbourhood(filterCoreSize, filterCoreSize);

				switch (filterCoreSize) {
				case 3: {
				  cimg_for3x3(inputImageRef, x, y, 0, 0, neighbourhood, ImageType) {
					(*result_image)(x,y) = interpolateInternal(inputImageRef(x,y), neighbourhood, absoluteDetectionThreshold, thresholdDirection);
				  }
				  break;
				}
				case 5: {
				  cimg_for5x5(inputImageRef, x, y, 0, 0, neighbourhood, ImageType) {
					(*result_image)(x,y) = interpolateInternal(inputImageRef(x,y), neighbourhood, absoluteDetectionThreshold, thresholdDirection);
				  }
				  break;
				}
				case 7: {
				  cimg_for7x7(inputImageRef, x, y, 0, 0, neighbourhood, ImageType) {
					(*result_image)(x,y) = interpolateInternal(inputImageRef(x,y), neighbourhood, absoluteDetectionThreshold, thresholdDirection);
				  }
				  break;
				}
				case 9: {
				  cimg_for9x9(inputImageRef, x, y, 0, 0, neighbourhood, ImageType) {
					(*result_image)(x,y) = interpolateInternal(inputImageRef(x,y), neighbourhood, absoluteDetectionThreshold, thresholdDirection);
				  }
				  break;
				}
				  //default:
				  // TODO: throw...
				  
				}
				
                DEBUG_IMAGE_DISPLAY(*result_image, "interpolate_bad_pixels_out", STARMATH_INTERPOLATE_BAD_PIXELS_DEBUG);

                return result_image;
            }
        );
    }

}

#endif // STARMATH_INTERPOLATE_BAD_PIXELS_H
