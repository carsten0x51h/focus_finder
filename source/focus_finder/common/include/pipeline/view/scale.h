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
     * InterpolationTypeT specifies the method which should be used to
     * interpolate during the scaling process.
     *
     *  https://cimg.eu/reference/structcimg__library_1_1CImg.html
     *       resize 	( 	const int  	size_x,
     *       const int  	size_y = -100,
     *       const int  	size_z = -100,
     *       const int  	size_c = -100,
     *       const int  	interpolation_type = 1
     *         ...
     *
     * interpolation_type	Method of interpolation:
     *
     *  -1 = no interpolation: raw memory resizing.
     *   0 = no interpolation: additional space is filled according to boundary_conditions.
     *   1 = nearest-neighbor interpolation.
     *   2 = moving average interpolation.
     *   3 = linear interpolation.
     *   4 = grid interpolation.
     *   5 = cubic interpolation.
     *   6 = lanczos interpolation.
     */
    struct InterpolationTypeT {
        enum TypeE {
            RAW,
            NO_INTERPOLATION,
            NEAREST_NEIGHBOR,
            MOVING_AVERAGE,
            LINEAR,
            GRID,
            CUBIC,
            LANCZOS,
            _Count
        };

        static const char *asStr(const TypeE &inType) {
            switch (inType) {
                case RAW:
                    return "RAW";
                case NO_INTERPOLATION:
                    return "NO_INTERPOLATION";
                case NEAREST_NEIGHBOR:
                    return "NEAREST_NEIGHBOR";
                case MOVING_AVERAGE:
                    return "MOVING_AVERAGE";
                case LINEAR:
                    return "LINEAR";
                case GRID:
                    return "GRID";
                case CUBIC:
                    return "CUBIC";
                case LANCZOS:
                    return "LANCZOS";
                default:
                    return "<?>";
            }
        }

        static const int asInt(const TypeE &inType) {
            switch (inType) {
                case RAW:
                    return -1;
                case NO_INTERPOLATION:
                    return 0;
                case NEAREST_NEIGHBOR:
                    return 1;
                case MOVING_AVERAGE:
                    return 2;
                case LINEAR:
                    return 3;
                case GRID:
                    return 4;
                case CUBIC:
                    return 5;
                case LANCZOS:
                    return 6;
                default:
                    return 1;
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
    scale(ScaleTypeT::TypeE scaleType, float scaleFactor, InterpolationTypeT::TypeE interpolation_type)
    {
        return ranges::views::transform(
                [=](const std::shared_ptr<cimg_library::CImg<ImageType> > & image) {

                    const cimg_library::CImg<ImageType> & inputImageRef = *image;

                    DEBUG_IMAGE_DISPLAY(inputImageRef, "scale_in", FOFI_SCALE_DEBUG);

                    auto scaledImage = std::make_shared<cimg_library::CImg<ImageType> >(inputImageRef);

                    float factor = (scaleType == ScaleTypeT::UP ? scaleFactor : 1.0F / scaleFactor);

                    scaledImage->resize((int) (factor * (float) inputImageRef.width()),
                                        (int) (factor * (float) inputImageRef.height()),
                                        -100,
                                        -100,
                                        InterpolationTypeT::asInt(interpolation_type));

                    DEBUG_IMAGE_DISPLAY(inputImageRef, "scale_out", FOFI_SCALE_DEBUG);

                    return scaledImage;
                }
        );
    }

    /**
     *
     * @tparam ImageType
     * @param scaleFactor
     * @param interpolation_type
     * @return
     */
    template<typename ImageType=float>
    auto
    scale_up(float scaleFactor, InterpolationTypeT::TypeE interpolation_type = InterpolationTypeT::NEAREST_NEIGHBOR) {
        return scale<ImageType>(ScaleTypeT::UP, scaleFactor, interpolation_type);
    }


    /**
     *
     * @tparam ImageType
     * @param scaleFactor
     * @param interpolation_type
     * @return
     */
    template<typename ImageType=float>
    auto
    scale_down(float scaleFactor, InterpolationTypeT::TypeE interpolation_type = InterpolationTypeT::NEAREST_NEIGHBOR) {
        return scale<ImageType>(ScaleTypeT::DOWN, scaleFactor, interpolation_type);
    }

} // End namespace AstroImagePipeline

#endif //FOFI_SCALE_H
