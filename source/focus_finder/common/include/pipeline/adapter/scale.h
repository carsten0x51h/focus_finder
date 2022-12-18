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

#ifndef FOFI_SCALE_H
#define FOFI_SCALE_H

#include <range/v3/view/transform.hpp>

#include "../../image.h"
#include "../../enum_helper.h"

#define FOFI_SCALE_DEBUG 0

namespace AstroImagePipeline {


    /**
     *
     */
    struct ScaleTypeT {
        enum TypeE {
            UP,
            DOWN,
            _Count
        };

        static const char *asStr(const TypeE &inType) {
            switch (inType) {
                case UP:
                    return "UP";
                case DOWN:
                    return "DOWN";
                default:
                    return "<?>";
            }
        }

        MAC_AS_TYPE(Type, E, _Count);
    };


    /**
     *
     * @tparam ImageType
     * @param scaleType
     * @param scaleFactor
     * @return
     */
    template<typename ImageType=float>
    auto
    scale(ScaleTypeT::TypeE scaleType, float scaleFactor)
    {
        return ranges::views::transform(
                [=](const std::shared_ptr<cimg_library::CImg<ImageType> > & image) {

                    const cimg_library::CImg<ImageType> & inputImageRef = *image;

                    DEBUG_IMAGE_DISPLAY(inputImageRef, "scale_in", FOFI_SCALE_DEBUG);

                    auto scaledImage = std::make_shared<cimg_library::CImg<ImageType> >(inputImageRef);

                    // TODO: Pass interpolation types...
                    // https://cimg.eu/reference/structcimg__library_1_1CImg.html
                    //            resize 	( 	const int  	size_x,
                    //            const int  	size_y = -100,
                    //            const int  	size_z = -100,
                    //            const int  	size_c = -100,
                    //            const int  	interpolation_type = 1
                    //              ...

                    float factor = (scaleType == ScaleTypeT::UP ? scaleFactor : 1.0F / scaleFactor);

                    scaledImage->resize((int) (factor * (float) inputImageRef.width()),
                                        (int) (factor * (float) inputImageRef.height()));

                    DEBUG_IMAGE_DISPLAY(inputImageRef, "scale_out", FOFI_SCALE_DEBUG);

                    return scaledImage;
                }
        );
    }

    /**
     *
     * @tparam ImageType
     * @param scaleFactor
     * @return
     */
    template<typename ImageType=float>
    auto
    scale_up(float scaleFactor) {
        return scale<ImageType>(ScaleTypeT::UP, scaleFactor);
    }

    /**
     *
     * @tparam ImageType
     * @param scaleFactor
     * @return
     */
    template<typename ImageType=float>
    auto
    scale_down(float scaleFactor) {
        return scale<ImageType>(ScaleTypeT::DOWN, scaleFactor);
    }

} // End namespace AstroImagePipeline

#endif //FOFI_SCALE_H
