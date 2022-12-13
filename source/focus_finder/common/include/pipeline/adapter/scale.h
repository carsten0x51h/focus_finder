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

#ifndef FOFI_SCALE_H
#define FOFI_SCALE_H

#include <boost/range/adaptors.hpp>

#include "../../image.h"

namespace AstroImagePipeline {


    template<typename Value>
    class scale_value {
    public:
        typedef const float argument_type;
        typedef const std::shared_ptr<ImageT> &result_type;

        scale_value(argument_type &scale_factor, argument_type &to)
                : m_scale_factor(scale_factor), m_to(to) {
        }

        //const Value &operator()(const Value &x) const {
        std::shared_ptr<ImageT> operator()(const Value &image) const {

            const ImageT & inputImageRef = *image;

            auto scaledImage = std::make_shared<ImageT>(inputImageRef, "xy");

            scaledImage->resize(m_scale_factor * inputImageRef.width(), m_scale_factor * inputImageRef.height());

            return scaledImage;
        }

    private:
        argument_type m_scale_factor;
        argument_type m_to;
    };

    template<typename Range>
    class scale_range
            : public boost::iterator_range<
                    boost::transform_iterator<
                            scale_value<typename boost::range_value<Range>::type>,
                            typename boost::range_iterator<Range>::type> > {
    private:
        typedef typename boost::range_value<Range>::type value_type;
        typedef typename boost::range_iterator<Range>::type iterator_base;
        typedef scale_value<value_type> Fn;
        typedef boost::transform_iterator<Fn, iterator_base> scale_iterator;
        typedef boost::iterator_range<scale_iterator> base_t;

    public:
        scale_range(Range &rng, float from, float to)
                : base_t(scale_iterator(boost::begin(rng), Fn(from, to)),
                         scale_iterator(boost::end(rng), Fn(from, to))) {
        }
    };


    template<typename T>
    class scale_holder : public boost::range_detail::holder2<T> {
    public:
        scale_holder(const T &from, const T &to)
                : boost::range_detail::holder2<T>(from, to) {}

    private:
        void operator=(const scale_holder &);
    };


    static boost::range_detail::forwarder2<scale_holder>
            scaleUp = boost::range_detail::forwarder2<scale_holder>();


    template<typename SinglePassRange>
    inline scale_range<SinglePassRange>
    operator|(SinglePassRange &rng,
              scale_holder<float> &f) {
        return scale_range<SinglePassRange>(rng, f.val1, f.val2);
    }


// const variant
//
    template<typename SinglePassRange>
    inline scale_range<const SinglePassRange>
    operator|(const SinglePassRange &rng,
              const scale_holder<float> &f) {
        return scale_range<const SinglePassRange>(rng, f.val1, f.val2);
    }

} // End namespace AstroImagePipeline

#endif //FOFI_SCALE_H
