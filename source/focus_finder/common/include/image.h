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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_IMAGE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_IMAGE_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_IMAGE_H_

#include <string>

/**
 * We do not want an X11 dependency by default.
 * Use DEFINE_IMAGE_DEBUG to enable CImg debug.
 */
#ifdef DEBUG_IMAGE_DISPLAY_SWITCH

    #define cimg_display 1

    #define DEBUG_IMAGE_DISPLAY(imageRef, title, enable)              \
      if (enable) {                                                   \
         cimg_library::CImgDisplay debugImageDisp((imageRef).width(),(imageRef).height(),title, 1); \
         debugImageDisp.display(imageRef);                            \
         while (! debugImageDisp.is_closed()) {                       \
           debugImageDisp.wait();                                     \
         }                                                            \
    }

#else
    #define cimg_display 0
    #define DEBUG_IMAGE_DISPLAY(Name, ImageRef, Title)  {}
#endif

/**
 * NOTE: In order to use TIFF, enable the following define. This also requires adding
 *       ${TIFF_LIBRARIES} to the CMakeLists.txt file.
 * See https://stackoverflow.com/questions/3021050/write-tiff-float-images-using-cimg
 *
 * Important:
 * "If you don't define cimg_use_tiff, then loading/saving tiff is not handled natively
 * by CImg, but is done through ImageMagick's convert, and in this case there are
 * limitations to 16bits tiff."
 *
 * See https://github.com/dtschump/CImg/issues/72
 */
#define cimg_use_tiff


/**
 * Enable CImg debug output
 */
#define cimg_verbosity 3

#include <CImg.h>

/**
 *
 */
// TODO: Introduce??
//Image8T
//Image16T
//ImageFT


/**
 * The template mechanism of the CImg class does not help here, since
 * the required bit depth of the image depends on the file to be loaded.
 * This happens at runtime and not at compile time. Therefore, for the
 * moment, the image type is fixed to float to support all possible
 * bit depths. In addition, the bit depth of the loaded image is stored
 * as part of the image,
 *
 *
 * Two options to potentially reduce memory usage are:
 *
 * Boost any: https://www.boost.org/doc/libs/1_54_0/doc/html/any/s02.html
 * Boost variant: https://www.boost.org/doc/libs/1_54_0/doc/html/variant.html
 */

using ImageT = cimg_library::CImg<float>;
//class ImageT : public cimg_library::CImg<float> {
//private:
//	unsigned int bitDepth_;
//
//public:
//	// See https://stackoverflow.com/questions/3119929/forwarding-all-constructors-in-c0x
//	using cimg_library::CImg<float>::CImg;
//
//	unsigned int getBitDepth() const { return bitDepth_; }
//	void setBitDepth(unsigned int bitDepth) { bitDepth_ = bitDepth; }
//
//	// copy assignment
//	ImageT& operator=(const cimg_library::CImg<float>& other)
//	{
//	    // Guard self assignment
//	    if (((cimg_library::CImg<float>*) this) == &other)
//	        return *this;
//
//	    *((cimg_library::CImg<float>*) this) = other;
//
//	    return *this;
//	}
//};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_IMAGE_H_ */


