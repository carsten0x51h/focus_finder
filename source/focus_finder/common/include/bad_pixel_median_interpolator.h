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

#ifndef STARMATH_BAD_PIXEL_MEDIAN_INTERPOLATOR_H
#define STARMATH_BAD_PIXEL_MEDIAN_INTERPOLATOR_H STARMATH_BAD_PIXEL_MEDIAN_INTERPOLATOR_H

/**
 * Usage:
 *
 * BadPixelMedianInterpolatorT badPixelMedianInterpolator();
 * auto outputImage = badPixelMedianInterpolator.interpolate(inputImage);
 *
 */
class BadPixelMedianInterpolatorT {
public:
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
   *
   */
  explicit BadPixelMedianInterpolatorT(float absoluteDetectionThreshold = 500, unsigned int filterCoreSize = 3, ThresholdDirectionT::TypeE thresholdDirection = ThresholdDirectionT::BOTH) : absoluteDetectionThreshold_(absoluteDetectionThreshold), filterCoreSize_(filterCoreSize), thresholdDirection_(thresholdDirection) {
}

  
  /**
   *
   */
  template<typename ImageType>
  std::shared_ptr<cimg_library::CImg<ImageType>> interpolate(const cimg_library::CImg<ImageType> &inputImageRef) {
	
	// See https://cimg.eu/reference/loops_Using.html
	cimg_library::CImg<ImageType> neighbourhood(filterCoreSize_, filterCoreSize_);

	auto result_image = std::make_shared<cimg_library::CImg<ImageType>>(inputImageRef);

	switch (filterCoreSize_) {
	case 3: {
	  cimg_for3x3(inputImageRef, x, y, 0, 0, neighbourhood, ImageType) {
		(*result_image)(x,y) = interpolateInternal(inputImageRef(x,y), neighbourhood, absoluteDetectionThreshold_, thresholdDirection_);
	  }
	  break;
	}
	case 5: {
	  cimg_for5x5(inputImageRef, x, y, 0, 0, neighbourhood, ImageType) {
		(*result_image)(x,y) = interpolateInternal(inputImageRef(x,y), neighbourhood, absoluteDetectionThreshold_, thresholdDirection_);
	  }
	  break;
	}
	case 7: {
	  cimg_for7x7(inputImageRef, x, y, 0, 0, neighbourhood, ImageType) {
		(*result_image)(x,y) = interpolateInternal(inputImageRef(x,y), neighbourhood, absoluteDetectionThreshold_, thresholdDirection_);
	  }
	  break;
	}
	case 9: {
	  cimg_for9x9(inputImageRef, x, y, 0, 0, neighbourhood, ImageType) {
		(*result_image)(x,y) = interpolateInternal(inputImageRef(x,y), neighbourhood, absoluteDetectionThreshold_, thresholdDirection_);
	  }
	  break;
	}
	  //default:
	  // TODO: throw...
				  
	}
	return result_image;
  }


	
 private:
	float absoluteDetectionThreshold_;
	unsigned int filterCoreSize_;
	ThresholdDirectionT::TypeE thresholdDirection_;
	  
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

};

#endif /*STARMATH_BAD_PIXEL_MEDIAN_INTERPOLATOR_H*/
