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

#ifndef FOFI_CENTER_ON_STAR_H
#define FOFI_CENTER_ON_STAR_H

#include <range/v3/view/transform.hpp>

#include "../../image.h"
#include "../../enum_helper.h"
#include "../../centroid_algorithm.h"
#include "../../rect.h"

#define FOFI_CENTER_ON_STAR_DEBUG 0

namespace AstroImagePipeline {

    template<typename ImageType=float>
    auto
    center_on_star(const std::shared_ptr<CentroidAlgorithmT> & centroid_algorithm) {
        return ranges::views::transform(
                [=](const std::shared_ptr<cimg_library::CImg<ImageType>> &image) {
                    // TODO: How to handle case when no centroid could be determined?
                    const cimg_library::CImg<ImageType> & inputImageRef = *image;

                    auto optCentroid = centroid_algorithm->calc(inputImageRef);

                    DEBUG_IMAGE_DISPLAY(inputImageRef, "center_on_star_in", FOFI_CENTER_ON_STAR_DEBUG);


                    if (optCentroid.has_value()) {
                        auto outerRoi = RectT<float>::fromCenterPoint(optCentroid.value(),
                                                                      (float) inputImageRef.width(),
                                                                      (float) inputImageRef.height()).to<int>();

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
                        auto centroidSubImg = std::make_shared<cimg_library::CImg<ImageType>>(
                                inputImageRef.get_crop(
                                        outerRoi.x() /*x0*/, outerRoi.y() /*y0*/,
                                        outerRoi.x() + outerRoi.width() - 1/*x1*/,
                                        outerRoi.y() + outerRoi.height() - 1/*y1*/)
                        );

                        DEBUG_IMAGE_DISPLAY(*centroidSubImg, "center_on_star_out", FOFI_CENTER_ON_STAR_DEBUG);

                        return centroidSubImg;
                    }
                    else {
                        // TODO: Take out the image?! Throw exception?
                        throw CentroidExceptionT("Unable to determine centroid.");
                    }
                }
        );
    }

} // End namespace AstroImagePipeline


#endif //FOFI_CENTER_ON_STAR_H
