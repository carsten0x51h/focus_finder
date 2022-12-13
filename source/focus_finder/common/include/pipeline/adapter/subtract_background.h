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

#ifndef FOFI_SUBTRACT_BACKGROUND_H
#define FOFI_SUBTRACT_BACKGROUND_H

#include <boost/range/adaptors.hpp>

#include "../../image.h"
#include "../../thresholding_algorithm_type.h"

namespace AstroImagePipeline {


    template<typename Value>
    class subtract_background_value {
    public:
        typedef const typename ThresholdingAlgorithmTypeT::TypeE argument_type;
        typedef const ImageT &result_type;

        subtract_background_value(argument_type &from, argument_type &to)
                : m_from(from), m_to(to) {
        }

        //const Value &operator()(const Value &x) const {
        ImageT operator()(const Value &image) const {

            std::cerr << "from: " << m_from << std::endl;

            std::cerr << "x: " << image.width() << std::endl;
            return image;
        }

    private:
//        Value m_from;
//        Value m_to;
        argument_type m_from;
        argument_type m_to;
    };

    template<typename Range>
    class subtract_background_range
            : public boost::iterator_range<
                    boost::transform_iterator<
                    subtract_background_value<typename boost::range_value<Range>::type>,
                    typename boost::range_iterator<Range>::type> > {
    private:
    typedef typename boost::range_value<Range>::type value_type;
    typedef typename boost::range_iterator<Range>::type iterator_base;
    typedef subtract_background_value<value_type> Fn;
    typedef boost::transform_iterator<Fn, iterator_base> subtract_background_iterator;
    typedef boost::iterator_range<subtract_background_iterator> base_t;

    public:
        //subtract_background_range(Range &rng, value_type from, value_type to)
    subtract_background_range(Range &rng, typename ThresholdingAlgorithmTypeT::TypeE from, typename ThresholdingAlgorithmTypeT::TypeE to)
            : base_t(subtract_background_iterator(boost::begin(rng), Fn(from, to)),
                     subtract_background_iterator(boost::end(rng), Fn(from, to))) {
    }
};

//template<typename T>
//class subtract_background_holder : public boost::range_detail::holder2<T> {
//public:
//    subtract_background_holder(const T &from, const T &to)
//            : boost::range_detail::holder2<T>(from, to) {}
//
//private:
//    void operator=(const subtract_background_holder &);
//};

template<typename T=typename ThresholdingAlgorithmTypeT::TypeE>
class subtract_background_holder : public boost::range_detail::holder2<T> {
public:
    subtract_background_holder(const T &from, const T &to)
            : boost::range_detail::holder2<T>(from, to) {}

private:
    void operator=(const subtract_background_holder &);
};


static boost::range_detail::forwarder2<subtract_background_holder>
        subtract_background = boost::range_detail::forwarder2<subtract_background_holder>();


template<typename SinglePassRange>
inline subtract_background_range<SinglePassRange>
operator|(SinglePassRange &rng,
          const subtract_background_holder<typename ThresholdingAlgorithmTypeT::TypeE /*typename boost::range_value<SinglePassRange>::type>*/> &f) {
    return subtract_background_range<SinglePassRange>(rng, f.val1, f.val2);
}


template<typename SinglePassRange>
inline subtract_background_range<const SinglePassRange>
operator|(const SinglePassRange &rng,
          const subtract_background_holder<typename ThresholdingAlgorithmTypeT::TypeE/*typename boost::range_value<SinglePassRange>::type*/> &f) {

    return subtract_background_range<const SinglePassRange>(rng, f.val1, f.val2);
}

} // End namespace AstroImagePipeline

#endif //FOFI_SUBTRACT_BACKGROUND_H
