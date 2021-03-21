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

// See https://en.cppreference.com/w/cpp/ranges
// TODO: Noet yet part of gcc-9
// See https://stackoverflow.com/questions/56118941/do-we-have-c20-ranges-library-in-gcc-9
//#include <ranges>
// Using boost as long as ranges lib made it into the std.
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>

#include "include/throw_if.h"
#include "include/outlier_filter.h"


OutlierFilterT::OutlierFilterT() : mOutlierBoundary(-1) {
}

void OutlierFilterT::setOutlierBoundary(float outlierBoundary) {
    mOutlierBoundary = outlierBoundary;
}

bool OutlierFilterT::isSet() const {
    return (mOutlierBoundary >= 0);
}

std::vector<PointWithResidualT> OutlierFilterT::getOutliers() const {
    return mOutliers;
}
