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

#ifndef FOFI_STAR_CLUSTER_H
#define FOFI_STAR_CLUSTER_H

#include <range/v3/view/transform.hpp>

#include "../../image.h"
#include "../../point.h"
#include "../../rect.h"
#include "../../size.h"

// TODO: Better pass star cluster algorithm type  (like for thresholding alogrithm) instead of including it directly here...
#include "../../star_cluster_algorithm.h"

#define FOFI_STAR_CLUSTER_DEBUG 0

namespace starmath::pipeline {

// See fs_dir.h <--- definition of directory_iterator...
//    inline star_cluster_iterator
//    begin(star_cluster_iterator __iter) noexcept
//    { return __iter; }
//
//    /// Return a past-the-end directory_iterator
//    inline star_cluster_iterator
//    end(star_cluster_iterator) noexcept
//    { return star_cluster_iterator(); }

// NOTE: directory_iterator also holds a private shared_ptr!:     std::__shared_ptr<_Dir_stack> _M_dirs;
// -> We build a class "star_cluster_iterator" which takes the "big" input image as shared_ptr and stores it and
//    takes the desired ThresholdingType as input parameters. The iterator automatically calculates a binary
//    image from the original one (also shared ptr). Using this iterator, one can move forward and backwards
//    through the image. The iterator returns a new std::shared_ptr<ImageT> of the clustered image (calculates
//    bounding box of all pixels in the cluster and then calls get_crop(bonding-box) on the original image).
//    The iterator points to this newly created shared-pointer image. The operation described above should take
//    place in the operator++ of the iterator implementation. Question is, if not maybe the algorithm should run
//    once and instead of the threshold-image, a copy of the result-cluster-list can be hold (also as shared-ptr)...

// In the end this must be possible
//    ranges::make_iterator_range(begin(starmath::star_cluster_iterator(img)),
//                                    end(starmath::star_cluster_iterator(img))) | ...


    template<typename ImageType=float>
    auto
    star_cluster(int x) {
        return ranges::views::transform(
                [=](const std::shared_ptr<cimg_library::CImg<ImageType> > &binaryImage) {

                    const cimg_library::CImg<ImageType> &binaryImageRef = *binaryImage;

                    DEBUG_IMAGE_DISPLAY(binaryImageRef, "star_cluster_in", FOFI_STAR_CLUSTER_DEBUG);

                    //StarClusterAlgorithmT starClusterAlgorithm(2); // TODO: Do not hardcode

                    //auto clusterRes = starClusterAlgorithm.cluster(binaryImageRef);
                    //DEBUG_IMAGE_DISPLAY(*croppedImg, "star_cluster_out", FOFI_STAR_CLUSTER_DEBUG);

                    return binaryImage;
                }
        );
    }
}

#endif //FOFI_STAR_CLUSTER_H
