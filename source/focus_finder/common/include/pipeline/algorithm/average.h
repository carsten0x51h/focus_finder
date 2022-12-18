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

#ifndef FOFI_AVERAGE_H
#define FOFI_AVERAGE_H

#include <range/v3/empty.hpp>

#include <memory>

#include "../../image.h"

template <class Rng>
std::shared_ptr<ImageT>
average(const Rng & rng) {

    if (ranges::empty(rng)) {
        return nullptr ; // TODO: Ok to return nullptr? Or throw?
    }

    auto sumImagePtr = std::make_shared<ImageT>(**ranges::begin(rng), "xy", 0);
    size_t imageCount = 0;

    for(const auto & img : rng) {
        // TODO: Check if images have different sizes? -> throw?
        (*sumImagePtr) += *img;
        ++imageCount;
    }

    (*sumImagePtr) /= imageCount;

    return sumImagePtr;
}

#endif //FOFI_AVERAGE_H
