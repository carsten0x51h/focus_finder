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

#include <boost/range/adaptors.hpp>

#include "../../image.h"
#include "../../enum_helper.h"
#include "../../centroid_algorithm.h"
#include "../../rect.h"

#define FOFI_CENTER_ON_STAR_DEBUG 1

namespace AstroImagePipeline {

    template<typename Value>
    class center_on_star_value {
    public:
        typedef const std::shared_ptr<CentroidAlgorithmT> argument_type1;
        typedef const std::shared_ptr<ImageT> &result_type;

        center_on_star_value(argument_type1 & centroid_algoritm)
                : m_centeroid_algorithm(centroid_algoritm) {
        }

        std::shared_ptr<ImageT> operator()(const Value &image) const {

            // TODO: How to handle case when no centroid could be determined?
            const ImageT & inputImageRef = *image;

            auto optCentroid = m_centeroid_algorithm->calc(inputImageRef);

            DEBUG_IMAGE_DISPLAY(inputImageRef, "center_on_star_in", FOFI_CENTER_ON_STAR_DEBUG);


            if (optCentroid.has_value()) {
                auto outerRoi = RectT<float>::fromCenterPoint(optCentroid.value(),
                                                              inputImageRef.width(),
                                                              inputImageRef.height()).to<int>();

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
                auto centroidSubImg = std::make_shared<ImageT>(
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
                //return image;
            }
        }

    private:
        argument_type1 m_centeroid_algorithm;
    };



    template<typename Range>
    class center_on_star_range
            : public boost::iterator_range<
                    boost::transform_iterator<
                            center_on_star_value<typename boost::range_value<Range>::type>,
                            typename boost::range_iterator<Range>::type> > {
    private:
        typedef typename boost::range_value<Range>::type value_type;
        typedef typename boost::range_iterator<Range>::type iterator_base;
        typedef center_on_star_value<value_type> Fn;
        typedef boost::transform_iterator<Fn, iterator_base> center_on_star_iterator;
        typedef boost::iterator_range<center_on_star_iterator> base_t;

    public:
        center_on_star_range(Range &rng, const std::shared_ptr<CentroidAlgorithmT> centroid_algorithm)
                : base_t(center_on_star_iterator(boost::begin(rng), Fn(centroid_algorithm)),
                         center_on_star_iterator(boost::end(rng), Fn(centroid_algorithm))) {
        }
    };

    template <class T>
    class center_on_star_holder {
    public:
        explicit center_on_star_holder(std::shared_ptr<CentroidAlgorithmT> & centroid_algorithm) : centroid_algorithm(centroid_algorithm) {}
        std::shared_ptr<CentroidAlgorithmT> centroid_algorithm;
        void operator=(const center_on_star_holder &) = delete;
    };


    static boost::range_detail::forwarder<center_on_star_holder>
            centerOnStar = boost::range_detail::forwarder<center_on_star_holder>();

    template<typename SinglePassRange>
    inline center_on_star_range<SinglePassRange>
    operator|(SinglePassRange &rng,
              center_on_star_holder<std::shared_ptr<CentroidAlgorithmT> > &f) {
        return center_on_star_range<SinglePassRange>(rng, f.centroid_algorithm);
    }


// const variant
//
    template<typename SinglePassRange>
    inline center_on_star_range<const SinglePassRange>
    operator|(const SinglePassRange &rng,
              const center_on_star_holder<std::shared_ptr<CentroidAlgorithmT> > &f) {
        return center_on_star_range<const SinglePassRange>(rng, f.centroid_algorithm);
    }

} // End namespace AstroImagePipeline


#endif //FOFI_CENTER_ON_STAR_H
