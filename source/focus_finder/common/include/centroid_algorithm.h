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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CENTROID_ALGORITHM_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CENTROID_ALGORITHM_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CENTROID_ALGORITHM_H_

#include <optional>
#include <functional>

#include "image.h"
#include "point.h"
#include "exception.h"

DEF_Exception(Centroid);

class CentroidAlgorithmT {
public:
    typedef std::function<double(const ImageT&)> BackgroundThresholdFunctionT;

    virtual ~CentroidAlgorithmT() = default;

    [[nodiscard]] virtual std::string getName() const = 0;

    [[nodiscard]] virtual std::optional<PointT<float>> calc(const ImageT &inImg, const BackgroundThresholdFunctionT& inBgThresholdFunction = nullptr) const = 0;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CENTROID_ALGORITHM_H_*/


