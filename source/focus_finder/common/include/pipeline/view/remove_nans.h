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

#ifndef FOFI_REMOVE_NANS_H
#define FOFI_REMOVE_NANS_H

#include <range/v3/view/transform.hpp>

#include "../../image.h"
#include "../../enum_helper.h"
#include "../../rect.h"

#define FOFI_REMOVE_NANS_DEBUG 0

namespace starmath::pipeline {

    template<typename ImageType=float>
    auto
    remove_nans() {
        return ranges::views::transform(
            [=](const std::shared_ptr<cimg_library::CImg<ImageType>> &image) {
	      
                const cimg_library::CImg<ImageType> &input_image_ref = *image;

                DEBUG_IMAGE_DISPLAY(input_image_ref, "remove_nans_in", FOFI_REMOVE_NANS_DEBUG);

		// Copy the existing image before making changes (TODO: Is this always needed?)
		auto result_image = std::make_shared<cimg_library::CImg<ImageType>>(input_image_ref);

		cimg_forXY(input_image_ref, x, y) {
		  if ( std::isnan(input_image_ref(x,y)) ) {
		    std::cerr << "FOUND NAN at (x,y)=(" << x << ", " << y << ")" << std::endl;

		    // HACK / TODO: Interpolate
		    (*result_image)(x,y) = 1.0f;
		  }
		}

		//inputImageRef.get_blur_median(3);
		
		DEBUG_IMAGE_DISPLAY(*result_image, "remove_nans_out", FOFI_REMOVE_NANS_DEBUG);

		return result_image;
            }
        );
    }
}

#endif //FOFI_REMOVE_NANS_H
