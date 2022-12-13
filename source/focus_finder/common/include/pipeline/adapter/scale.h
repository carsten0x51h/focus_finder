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
#include "../../enum_helper.h"

namespace AstroImagePipeline {


    struct ScaleTypeT {
        enum TypeE {
            UP,
            DOWN,
            _Count
        };

        static const char *asStr(const TypeE &inType) {
            switch (inType) {
                case UP:
                    return "UP";
                case DOWN:
                    return "DOWN";
                default:
                    return "<?>";
            }
        }

        MAC_AS_TYPE(Type, E, _Count);
    };



    template<typename Value>
    class scale_value {
    public:
        typedef const ScaleTypeT::TypeE argument_type1;
        typedef const float argument_type2;
        typedef const std::shared_ptr<ImageT> &result_type;

        scale_value(argument_type1 & scale_type, argument_type2 & scale_factor)
                : m_scale_type(scale_type), m_scale_factor(scale_factor) {
        }

        std::shared_ptr<ImageT> operator()(const Value &image) const {

            const ImageT & inputImageRef = *image;

            auto scaledImage = std::make_shared<ImageT>(inputImageRef, "xy");

            // TODO: Pass interpolation types...
            // https://cimg.eu/reference/structcimg__library_1_1CImg.html
            //            resize 	( 	const int  	size_x,
            //            const int  	size_y = -100,
            //            const int  	size_z = -100,
            //            const int  	size_c = -100,
            //            const int  	interpolation_type = 1
            //              ...

            float factor = (m_scale_type == ScaleTypeT::UP ? m_scale_factor : 1.0F / m_scale_factor);

            scaledImage->resize(factor * inputImageRef.width(), factor * inputImageRef.height());

            return scaledImage;
        }

    private:
        argument_type1 m_scale_type;
        argument_type2 m_scale_factor;
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
        scale_range(Range &rng, ScaleTypeT::TypeE scaleType, float scaleFactor)
                : base_t(scale_iterator(boost::begin(rng), Fn(scaleType, scaleFactor)),
                         scale_iterator(boost::end(rng), Fn(scaleType, scaleFactor))) {
        }
    };



    template<typename T>
    class scale_up_holder {
    public:
        explicit scale_up_holder(const T &scale_factor) : scale_factor(scale_factor) {}

        T scale_factor;

    private:
        void operator=(const scale_up_holder &) = delete;
    };


    template<typename T>
    class scale_down_holder {
    public:
        explicit scale_down_holder(const T &scale_factor) : scale_factor(scale_factor) {}

        T scale_factor;

    private:
        void operator=(const scale_down_holder &) = delete;
    };


    static boost::range_detail::forwarder<scale_up_holder>
            scaleUp = boost::range_detail::forwarder<scale_up_holder>();

    static boost::range_detail::forwarder<scale_down_holder>
            scaleDown = boost::range_detail::forwarder<scale_down_holder>();

    template<typename SinglePassRange>
    inline scale_range<SinglePassRange>
    operator|(SinglePassRange &rng,
              scale_up_holder<float> &f) {
        return scale_range<SinglePassRange>(rng, ScaleTypeT::UP, f.scale_factor);
    }


// const variant
//
    template<typename SinglePassRange>
    inline scale_range<const SinglePassRange>
    operator|(const SinglePassRange &rng,
              const scale_up_holder<float> &f) {
        return scale_range<const SinglePassRange>(rng, ScaleTypeT::UP, f.scale_factor);
    }


    template<typename SinglePassRange>
    inline scale_range<SinglePassRange>
    operator|(SinglePassRange &rng,
              scale_down_holder<float> &f) {
        return scale_range<SinglePassRange>(rng, ScaleTypeT::DOWN, f.scale_factor);
    }


// const variant
//
    template<typename SinglePassRange>
    inline scale_range<const SinglePassRange>
    operator|(const SinglePassRange &rng,
              const scale_down_holder<float> &f) {
        return scale_range<const SinglePassRange>(rng, ScaleTypeT::DOWN, f.scale_factor);
    }

} // End namespace AstroImagePipeline

#endif //FOFI_SCALE_H
