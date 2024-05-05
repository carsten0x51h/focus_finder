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

#ifndef STARMATH_INCLUDE_PIPELINE_VIEW_WRITE_H_
#define STARMATH_INCLUDE_PIPELINE_VIEW_WRITE_H_ STARMATH_INCLUDE_PIPELINE_VIEW_WRITE_H_


#include <range/v3/view/transform.hpp>

#include "../../image.h"
#include "../../image_writer.h"

#define STARMATH_PIPELINE_WRITE_DEBUG 0

// TODO: Maybe this is an action? Sould this write() return the same image again to potentially allow continuing
//       the pipeline workflow?
namespace starmath::pipeline::io {

	/**
	 * TODO: Rename to read()
	 * TODO: Idea... maybe rename to starmath::pipeline::io ?
	 */
    template<typename ImageType=float>
    auto
    write(const std::string &imageFilenamePattern, bool allowOverride = true) {
        return ranges::views::transform(
            [=](const std::shared_ptr<ImageT> & image) {

        		static unsigned int counter = 0;

        		DEBUG_IMAGE_DISPLAY(*image, "pipeline_write_in", STARMATH_PIPELINE_WRITE_DEBUG );

        		// TODO: Compose filename from internal, static counter... ? Better idea? Reset?
        		std::string filename = imageFilenamePattern; // .... TODO ... generate filename from pattern

                starmath::io::write(*image, filename, allowOverride);

                return image;
            }
        );
    }
}

#endif /* STARMATH_INCLUDE_PIPELINE_VIEW_WRITE_H_ */
