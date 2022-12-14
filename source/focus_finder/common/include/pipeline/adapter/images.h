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

#include <boost/range/adaptors.hpp>

#include "../../image.h"

#define FOFI_IMAGES_DEBUG 0

namespace AstroImagePipeline {


    template<typename Value>
    class images_value {
    public:
        typedef const int argument_type;
        typedef const std::shared_ptr<ImageT> &result_type;

        images_value(argument_type &from, argument_type &to)
                : m_from(from), m_to(to) {
        }

        std::shared_ptr<ImageT> operator()(const Value &imageFilename) const {

            auto loadedImage = std::make_shared<ImageT>(imageFilename.c_str());

            DEBUG_IMAGE_DISPLAY(*loadedImage, "images_out", FOFI_IMAGES_DEBUG);

            return loadedImage;
        }

    private:
        argument_type m_from;
        argument_type m_to;
    };

    template<typename Range>
    class images_range
            : public boost::iterator_range<
                    boost::transform_iterator<
                    images_value<typename boost::range_value<Range>::type>,
                    typename boost::range_iterator<Range>::type> > {
    private:
    typedef typename boost::range_value<Range>::type value_type;
    typedef typename boost::range_iterator<Range>::type iterator_base;
    typedef images_value<value_type> Fn;
    typedef boost::transform_iterator<Fn, iterator_base> images_iterator;
    typedef boost::iterator_range<images_iterator> base_t;

    public:
        //images_range(Range &rng, value_type from, value_type to)
    images_range(Range &rng, int from, int to)
            : base_t(images_iterator(boost::begin(rng), Fn(from, to)),
                     images_iterator(boost::end(rng), Fn(from, to))) {
    }
};


template<typename T>
class images_holder : public boost::range_detail::holder2<T> {
public:
    images_holder(const T &from, const T &to)
            : boost::range_detail::holder2<T>(from, to) {}

private:
    void operator=(const images_holder &) = delete;
};


static boost::range_detail::forwarder2<images_holder>
        images = boost::range_detail::forwarder2<images_holder>();


template<typename SinglePassRange>
inline images_range<SinglePassRange>
operator|(SinglePassRange &rng,
          images_holder<int /*typename boost::range_value<SinglePassRange>::type>*/> &f) {
    return images_range<SinglePassRange>(rng, f.val1, f.val2);
}


// const variant
//
template<typename SinglePassRange>
inline images_range<const SinglePassRange>
operator|(const SinglePassRange &rng,
          const images_holder<int/*typename boost::range_value<SinglePassRange>::type*/> &f) {
    return images_range<const SinglePassRange>(rng, f.val1, f.val2);
}

} // End namespace AstroImagePipeline

#endif //FOFI_IMAGES_H
