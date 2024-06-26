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

#include <ostream>
#include <utility>

#include "include/point_with_residual.h"

PointWithResidualT::PointWithResidualT(PointFT point, float residual) : point(std::move(point)), residual(residual) {

}

std::ostream &operator<<(std::ostream &os,
                         const PointWithResidualT &pointWithResidual) {
    static const char *prefix = "   ";

    os << prefix << "** PointWithResidual **" << std::endl;
    os << prefix << " > Point (x,y)=(" << pointWithResidual.point << ") - residual=" << pointWithResidual.residual
       << std::endl;

    return os;
}
