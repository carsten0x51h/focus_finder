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

#include <memory>

#include "include/centroid_algorithm_factory.h"
#include "include/intensity_weighted_centroid_algorithm.h"

std::shared_ptr<CentroidAlgorithmT> CentroidAlgorithmFactoryT::getInstance(
        const CentroidAlgorithmTypeT::TypeE &type) {

    switch (type) {
        case CentroidAlgorithmTypeT::IWC:
            return std::make_shared<IntensityWeightedCentroidAlgorithmT>();

        default:
            return nullptr;
    }
}
