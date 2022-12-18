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

#ifndef FOFI_SUBTRACT_BACKGROUND_H
#define FOFI_SUBTRACT_BACKGROUND_H

#include <range/v3/view/transform.hpp>

#include "../../image.h"
#include "../../thresholding_algorithm.h"

#define FOFI_SUBTRACT_BACKGROUND_DEBUG 0


// IDEA: Maybe this can be split into a normal subtract and value_clip ?
//       -> No, because to calculate the threshold the actual image from the range is needed.
namespace AstroImagePipeline {

    template<typename ImageType=float>
    auto
    subtract_background(const std::shared_ptr<ThresholdingAlgorithmT> & thresholding_algorithm) {
        return ranges::views::transform(
            [=](const std::shared_ptr<ImageT> &image) {
                const ImageT & inputImageRef = *image;

                DEBUG_IMAGE_DISPLAY(inputImageRef, "subtract_background_in", FOFI_SUBTRACT_BACKGROUND_DEBUG);

                // TODO: Handle bit depth... do not hardcode here...
                float threshold = thresholding_algorithm->calc(inputImageRef, 16);

                auto subImage = std::make_shared<cimg_library::CImg<ImageType> >(inputImageRef, "xy");

                ImageT & subImageRef = (*subImage);

                cimg_forXY(inputImageRef, x, y) {
                    subImageRef(x, y) = (inputImageRef(x, y) < threshold ? 0 : inputImageRef(x, y) - threshold);
                }

                DEBUG_IMAGE_DISPLAY(subImageRef, "subtract_background_out", FOFI_SUBTRACT_BACKGROUND_DEBUG);

                return subImage;
            }
        );
    }

} // End namespace AstroImagePipeline

#endif //FOFI_SUBTRACT_BACKGROUND_H
