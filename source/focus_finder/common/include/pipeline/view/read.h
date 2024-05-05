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

#ifndef STARMATH_PIPELINE_IO_READ_H_
#define STARMATH_PIPELINE_IO_READ_H_ STARMATH_PIPELINE_IO_READ_H_

#include <range/v3/view/transform.hpp>

#include "../../image_reader.h"

#define FOFI_IMAGES_DEBUG 0

namespace starmath::pipeline::io {

	/**
	 *
	 */
    template<typename ImageType=float>
    auto
    read() {
        return ranges::views::transform(
            [=](const std::string &imageFilename) {

                // TODO: Change to
                // #include "image_reader.h"
                auto loadedImage = starmath::io::read(imageFilename);

                DEBUG_IMAGE_DISPLAY(*loadedImage, "images_out", FOFI_IMAGES_DEBUG);

                return loadedImage;
            }
        );
    }
}

#endif // STARMATH_PIPELINE_IO_READ_H_
