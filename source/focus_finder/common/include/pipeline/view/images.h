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

#ifndef FOFI_IMAGES_H
#define FOFI_IMAGES_H

#include <range/v3/view/transform.hpp>

#include "../../image.h"

#define FOFI_IMAGES_DEBUG 0

namespace AstroImagePipeline {

    template<typename ImageType=float>
    auto
    images() {
        return ranges::views::transform(
                [=](const std::string &imageFilename) {
                    auto loadedImage = std::make_shared<cimg_library::CImg<ImageType> >(imageFilename.c_str());

                    DEBUG_IMAGE_DISPLAY(*loadedImage, "images_out", FOFI_IMAGES_DEBUG);

                    return loadedImage;
                }
        );
    }
} // End namespace AstroImagePipeline

#endif //FOFI_IMAGES_H
