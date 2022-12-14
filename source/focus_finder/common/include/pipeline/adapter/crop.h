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

#include <boost/range/adaptors.hpp>

#include "../../image.h"
#include "../../size.h"

#define FOFI_CROP_DEBUG 1

namespace AstroImagePipeline {


    template<typename Value>
    class crop_value {
    public:
        typedef const SizeT<int> argument_type;
        typedef std::shared_ptr<ImageT> &result_type;

        explicit crop_value(const argument_type & crop_region) {
            m_crop_region = crop_region;
        }

        std::shared_ptr<ImageT> operator()(const Value & image) const {

            const ImageT & inputImageRef = *image;

            DEBUG_IMAGE_DISPLAY(inputImageRef, "crop_in", FOFI_CROP_DEBUG);

            // TODO: Maybe this calculation can be simplified...
            PointT<float> imageCenter(image->width() / 2.0F, image->height() / 2.0F);

            RectT<float> rect = RectT<float>::fromCenterPoint(imageCenter, m_crop_region.to<float>());


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
            auto croppedImg = std::make_shared<ImageT>(
                    inputImageRef.get_crop(
                            rect.x() /*x0*/, rect.y() /*y0*/,
                            rect.x() + rect.width() - 1/*x1*/,
                            rect.y() + rect.height() - 1/*y1*/)
            );

            DEBUG_IMAGE_DISPLAY(*croppedImg, "crop_out", FOFI_CROP_DEBUG);

            return croppedImg;
        }

    private:
        SizeT<int> m_crop_region;
    };

    template<typename Range>
    class crop_range
            : public boost::iterator_range<
                    boost::transform_iterator<
                            crop_value<typename boost::range_value<Range>::type>,
                            typename boost::range_iterator<Range>::type> > {
    private:
        typedef typename boost::range_value<Range>::type value_type;
        typedef typename boost::range_iterator<Range>::type iterator_base;
        typedef crop_value<value_type> Fn;
        typedef boost::transform_iterator<Fn, iterator_base> crop_iterator;
        typedef boost::iterator_range<crop_iterator> base_t;

    public:
        //crop_range(Range &rng, value_type from, value_type to)
        crop_range(Range &rng, SizeT<int> from)
                : base_t(crop_iterator(boost::begin(rng), Fn(from)),
                         crop_iterator(boost::end(rng), Fn(from))) {
        }
    };


    template<typename T>
    class crop_holder : public boost::range_detail::holder<T> {
    public:
        explicit crop_holder(const T &threshold_type)
                : boost::range_detail::holder<T>(threshold_type) {}

    private:
        void operator=(const crop_holder &) = delete;
    };


    static boost::range_detail::forwarder<crop_holder>
            crop_from_center = boost::range_detail::forwarder<crop_holder>();





    template<typename SinglePassRange>
    inline crop_range<SinglePassRange>
    operator|(SinglePassRange &rng,
              crop_holder<SizeT<int> /*typename boost::range_value<SinglePassRange>::type>*/> &f) {
        return crop_range<SinglePassRange>(rng, f.val);
    }


// const variant
//
    template<typename SinglePassRange>
    inline crop_range<const SinglePassRange>
    operator|(const SinglePassRange &rng,
              const crop_holder<SizeT<int>> &f) {

        return crop_range<const SinglePassRange>(rng, f.val);
    }

} // End namespace AstroImagePipeline


#endif //FOFI_CROP_H
