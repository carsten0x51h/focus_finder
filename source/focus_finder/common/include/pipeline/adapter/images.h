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

namespace AstroImagePipeline {
    template<typename Value>
    class replace_value {
    public:
        typedef const Value &result_type;
        typedef const Value &argument_type;

        replace_value(const Value &from, const Value &to)
                : m_from(from), m_to(to) {
        }

        const Value &operator()(const Value &x) const {
            return (x == m_from) ? m_to : x;
        }

    private:
        Value m_from;
        Value m_to;
    };

    template<typename Range>
    class replace_range
            : public boost::iterator_range<
                    boost::transform_iterator<
                    replace_value<typename boost::range_value<Range>::type>,
                    typename boost::range_iterator<Range>::type> > {
    private:
    typedef typename boost::range_value<Range>::type value_type;
    typedef typename boost::range_iterator<Range>::type iterator_base;
    typedef replace_value<value_type> Fn;
    typedef boost::transform_iterator<Fn, iterator_base> replaced_iterator;
    typedef boost::iterator_range<replaced_iterator> base_t;

    public:
    replace_range(Range &rng, value_type from, value_type to)
            : base_t(replaced_iterator(boost::begin(rng), Fn(from, to)),
                     replaced_iterator(boost::end(rng), Fn(from, to))) {
    }
};

template<typename T>
class replace_holder : public boost::range_detail::holder2<T> {
public:
    replace_holder(const T &from, const T &to)
            : boost::range_detail::holder2<T>(from, to) {}

private:
    void operator=(const replace_holder &);
};


static boost::range_detail::forwarder2<replace_holder>
        images = boost::range_detail::forwarder2<replace_holder>();


template<typename SinglePassRange>
inline replace_range<SinglePassRange>
operator|(SinglePassRange &rng,
          const replace_holder<typename boost::range_value<SinglePassRange>::type> &f) {
    return replace_range<SinglePassRange>(rng, f.val1, f.val2);
}


template<typename SinglePassRange>
inline replace_range<const SinglePassRange>
operator|(const SinglePassRange &rng,
          const replace_holder<typename boost::range_value<SinglePassRange>::type> &f) {
    return replace_range<const SinglePassRange>(rng, f.val1, f.val2);
}

} // End namespace AstroImagePipeline

#endif //FOFI_IMAGES_H
