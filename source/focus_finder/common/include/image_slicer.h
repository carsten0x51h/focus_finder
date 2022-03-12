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

#ifndef _IMAGE_SLICER_H_
#define _IMAGE_SLICER_H_ _IMAGE_SLICER_H_

#include <vector>

#include "logging.h"
#include "exception.h"
#include "image.h"
#include "enum_helper.h"

/**
 * Helper class - horizontal and vertical.
 */
struct SliceDirectionT {
    enum TypeE {
        HORZ, VERT, _Count
    };

    static const char *asStr(const TypeE &inType) {
        switch (inType) {
            case HORZ:
                return "HORZ";
            case VERT:
                return "VERT";
            default:
                return "<?>";
        }
    }

    MAC_AS_TYPE(Type, E, _Count);
};

DEF_Exception(ImageSlicer);

class ImageSlicerT {
public:
    template<SliceDirectionT::TypeE D>
    static std::vector<typename ImageT::value_type> slice(
            const ImageT &inImage, size_t index) {
        std::vector<typename ImageT::value_type> values;

        // In case an empty image is passed, return an empty vector.
        if (inImage.is_empty()) {
            return values;
        }

        // Extract slices through centroid for profiles
        switch (D) {
            case SliceDirectionT::HORZ: {
                if (index >= inImage.width()) {
                    throw ImageSlicerExceptionT("Index out of bounds.");
                }

                values.resize(inImage.width());

                cimg_forX(inImage, x) {
                    values[x] = inImage(x, index);
                }
                break;
            }
            case SliceDirectionT::VERT: {
                if (index >= inImage.height()) {
                    throw ImageSlicerExceptionT("Index out of bounds.");
                }

                values.resize(inImage.height());

                cimg_forY(inImage, y) {
                    values[y] = inImage(index, y);
                }
                break;
            }
            default: {
                throw ImageSlicerExceptionT("Invalid direction.");
            }
        }
        return values;
    }
};

#endif /*_IMAGE_SLICER_H_*/
