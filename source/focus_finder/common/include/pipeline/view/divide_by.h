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

#ifndef FOFI_DIVIDE_BY_H
#define FOFI_DIVIDE_BY_H

#include <range/v3/view/transform.hpp>

#include "../../image.h"

#define FOFI_DIVIDE_DEBUG 0

namespace starmath::pipeline {

    /**
     *
     * @tparam ImageType
     * @param divisorImagePtr
     * @return
     *
     * TODO: Check that images have same dimensions! Otherwise this will break!
     */
    template<typename ImageType=float>
    auto
    divide_by(const std::shared_ptr<ImageT> &divisorImagePtr) {
        return ranges::views::transform(
            [=](const std::shared_ptr<cimg_library::CImg<ImageType> > &image) {
                const cimg_library::CImg<ImageType> &inputImageRef = *image;

                DEBUG_IMAGE_DISPLAY(inputImageRef, "divide_image_in", FOFI_DIVIDE_DEBUG);

                auto result_image = std::make_shared<cimg_library::CImg<ImageType>>(
                        inputImageRef.get_div(*divisorImagePtr));

                DEBUG_IMAGE_DISPLAY(*result_image, "divide_image_out", FOFI_DIVIDE_DEBUG);

                return result_image;
            }
        );
    }


    /**
     *
     * @tparam ImageType
     * @param scalarDivisor
     * @return
     */
    template<typename ImageType=float>
    auto
    divide_by(ImageType scalarDivisor) {
        return ranges::views::transform(
            [=](const std::shared_ptr<cimg_library::CImg<ImageType> > &image) {
                const cimg_library::CImg<ImageType> &inputImageRef = *image;

                DEBUG_IMAGE_DISPLAY(inputImageRef, "divide_scalar_in", FOFI_DIVIDE_DEBUG);

                // TODO: FIXME! see above...
                auto result_image = std::make_shared<cimg_library::CImg<ImageType>>(
                        inputImageRef / scalarDivisor);

                DEBUG_IMAGE_DISPLAY(*result_image, "divide_scalar_out", FOFI_DIVIDE_DEBUG);

                return result_image;
            }
        );
    }
}

#endif //FOFI_DIVIDE_BY_H
