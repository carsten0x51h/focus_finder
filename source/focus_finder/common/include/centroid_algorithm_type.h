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


#ifndef FOFI_CENTROID_ALGORITHM_TYPE_H
#define FOFI_CENTROID_ALGORITHM_TYPE_H

#include "enum_helper.h"

/**
 * See http://cdn.intechopen.com/pdfs-wm/26716.pdf
 * for a description of the different centroiding algorithms.
 */
struct CentroidAlgorithmTypeT {
    enum TypeE {
        COG,  // Center of gravity
        WCOG, // Weighted center of gravity
        IWC,  // Intensity Weighted Centroiding
        _Count
    };

    static const char *asStr(const TypeE &inType) {
        switch (inType) {
            case COG:
                return "COG";
            case WCOG:
                return "WCOG";
            case IWC:
                return "IWC";
            default:
                return "<?>";
        }
    }

    MAC_AS_TYPE(Type, E, _Count);
};

#endif //FOFI_CENTROID_ALGORITHM_TYPE_H
