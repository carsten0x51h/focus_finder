/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c.schmitt51h@gmail.com>
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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TUPLE_PRINTER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TUPLE_PRINTER_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TUPLE_PRINTER_H_

#include <ostream>
#include <tuple>

// Define a type which holds an unsigned integer value
// See http://cpplove.blogspot.de/2012/07/printing-tuples.html
template<std::size_t>
struct int_ {
};

template<class Tuple, size_t Pos>
std::ostream &print_tuple(std::ostream &out, const Tuple &t, int_<Pos>) {
    out << std::get<std::tuple_size<Tuple>::value - Pos>(t) << ',';
    return print_tuple(out, t, int_<Pos - 1>());
}

template<class Tuple>
std::ostream &print_tuple(std::ostream &out, const Tuple &t, int_<1>) {
    auto &value = std::get<std::tuple_size<Tuple>::value - 1>(t);
    return out << value;
}

template<class... Args>
std::ostream &operator<<(std::ostream &out, const std::tuple<Args...> &t) {
    out << '(';
    print_tuple(out, t, int_<sizeof...(Args)>());
    return out << ')';
}

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TUPLE_PRINTER_H_ */
