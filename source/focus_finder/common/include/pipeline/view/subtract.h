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

#ifndef FOFI_SUBTRACT_H
#define FOFI_SUBTRACT_H

#include <range/v3/view/transform.hpp>

#include "../../image.h"

#define FOFI_SUBTRACT_DEBUG 0

namespace starmath::pipeline {

    /**
     *
     * @tparam ImageType
     * @param imageToSubtractPtr
     * @return
     *
     * TODO: Check that all images have the same dimension? Could be configured by parameter...
     */
    template<typename ImageType=float>
    auto
    subtract(const std::shared_ptr<ImageT> &imageToSubtractPtr) {
        return ranges::views::transform(
                [=](const std::shared_ptr<cimg_library::CImg<ImageType> > &image) {
                    const cimg_library::CImg<ImageType> &inputImageRef = *image;

                    DEBUG_IMAGE_DISPLAY(inputImageRef, "subtract_image_in", FOFI_SUBTRACT_DEBUG);

                    auto result_image = std::make_shared<cimg_library::CImg<ImageType>>(
                            inputImageRef - *imageToSubtractPtr);

                    DEBUG_IMAGE_DISPLAY(*result_image, "subtract_image_out", FOFI_SUBTRACT_DEBUG);

                    return result_image;
                }
        );
    }


    /**
     *
     * @tparam ImageType
     * @param scalarValueToSubtract
     * @return
     */
    template<typename ImageType=float>
    auto
    subtract(ImageType scalarValueToSubtract) {
        return ranges::views::transform(
                [=](const std::shared_ptr<cimg_library::CImg<ImageType> > &image) {
                    const cimg_library::CImg<ImageType> &inputImageRef = *image;

                    DEBUG_IMAGE_DISPLAY(inputImageRef, "subtract_scalar_in", FOFI_SUBTRACT_DEBUG);

                    auto result_image = std::make_shared<cimg_library::CImg<ImageType>>(
                            inputImageRef - scalarValueToSubtract);

                    DEBUG_IMAGE_DISPLAY(*result_image, "subtract_scalar_out", FOFI_SUBTRACT_DEBUG);

                    return result_image;
                }
        );
    }
}

#endif //FOFI_SUBTRACT_H
