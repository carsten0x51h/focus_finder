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

#ifndef FOFI_STAR_ANALYSIS_H
#define FOFI_STAR_ANALYSIS_H

#include "image.h"

#include <memory>

namespace StarAnalysis {
    /**
     *
     * See discussion at https://www.cloudynights.com/topic/698887-how-can-i-tell-if-i-am-saturating-pixels/
     *
     * @return
     */
    static bool isSaturated(const std::shared_ptr<ImageT> & image) {

        float bitDepth = 16.0F; // TODO: Handle bit depth!

        float minVal;
        float maxVal = image->max_min(minVal);

        return (maxVal >= std::pow(2.0F, bitDepth));
    }

    static bool isNotSaturated(const std::shared_ptr<ImageT> & image) {
        return ! isSaturated(image);
    }

};

#endif //FOFI_STAR_ANALYSIS_H
