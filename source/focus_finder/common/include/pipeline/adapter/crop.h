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

#ifndef FOFI_CROP_H
#define FOFI_CROP_H

#include <range/v3/view/transform.hpp>

#include "../../image.h"
#include "../../point.h"
#include "../../rect.h"
#include "../../size.h"

#define FOFI_CROP_DEBUG 0

namespace AstroImagePipeline {

    template<typename ImageType=float>
    auto
    crop_from_center(const SizeT<int> & crop_region)
    {
        return ranges::views::transform(
            [=](const std::shared_ptr<cimg_library::CImg<ImageType> > & image) {

                const cimg_library::CImg<ImageType> & inputImageRef = *image;

                DEBUG_IMAGE_DISPLAY(inputImageRef, "crop_from_center_in", FOFI_CROP_DEBUG);

                // TODO: Maybe this calculation can be simplified...
                PointT<float> imageCenter((float) inputImageRef.width() / 2.0F, (float) inputImageRef.height() / 2.0F);

                RectT<float> rect = RectT<float>::fromCenterPoint(imageCenter, crop_region.to<float>());


                // get_crop() by default applies a dirichlet boundary_condition (=0). There are other
                // options as well. In this case, the desired behaviour is to assume that all pixel values
                // where the defined sub-frame exceeds the image boundary are assumed to be 0.
                //
                // boundary_conditions	= Can be { 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }
                //
                // - Dirichlet means "0 outside image".
                // - Neumann means "Nearest neighbor outside image" (i.e. null derivative)
                // - Periodic means "Periodic"
                // - Mirror means "Mirrored image outside".
                //
                // See https://github.com/GreycLab/CImg/issues/110
                auto croppedImg = std::make_shared<cimg_library::CImg<ImageType>>(
                        inputImageRef.get_crop(
                                rect.x() /*x0*/, rect.y() /*y0*/,
                                rect.x() + rect.width() - 1/*x1*/,
                                rect.y() + rect.height() - 1/*y1*/)
                );

                DEBUG_IMAGE_DISPLAY(*croppedImg, "crop_from_center_out", FOFI_CROP_DEBUG);

                return croppedImg;
            }
        );
    }
} // End namespace AstroImagePipeline

#endif //FOFI_CROP_H
