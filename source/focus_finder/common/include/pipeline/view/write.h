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

#include <cstdio>
#include <filesystem>

/**
 * NOTE: With C++20, the filename can be easily composed like:
 * std::string filename = std::format(imageFilenamePattern, counter++);
 * The template looks slightly different: "img_{:03}.fit"
 */
//#include <format>

#include <range/v3/view/transform.hpp>

#include "../../image.h"
#include "../../image_writer.h"
#include "../../logging.h"

#define STARMATH_PIPELINE_WRITE_DEBUG 0

// TODO: Maybe this is an action? Sould this write() return the same image again to potentially allow continuing
//       the pipeline workflow?
namespace starmath::pipeline::io {


	/**
	 * NOTE: With C++20, the filename can be easily composed like:
	 * std::string filename = std::format(imageFilenamePattern, counter++);
	 * The template looks slightly different: "img_{:03}.fit"
	 *
	 * TODO: Compose filename from internal, static counter... ? Better idea? How to reset?
	 */
	static std::string compose_filename_from_pattern(const std::string & pattern) {
	    char buffer[100];

		static unsigned int counter = 0;

	    std::snprintf(buffer, sizeof(buffer), pattern.c_str(), counter++);

	    return std::string(buffer);
	}

	/**
	 * TODO: Rename to read()
	 * TODO: Idea... maybe rename to starmath::pipeline::io ?
	 */
    template<typename ImageType=float>
    auto
    write(const std::filesystem::path & directory, const std::string &imageFilenamePattern = "img_%03d.fit", bool allowOverride = true) {
        return ranges::views::transform(
            [=](const std::shared_ptr<ImageT> & image) {

        		DEBUG_IMAGE_DISPLAY(*image, "pipeline_write_in", STARMATH_PIPELINE_WRITE_DEBUG );

        		std::filesystem::path filepath = directory / compose_filename_from_pattern(imageFilenamePattern);

        		LOG(debug) << "starmath::pipeline::io::write() - filepath to store image to: " << filepath << std::endl;

                starmath::io::write(*image, filepath, allowOverride);

                return image;
            }
        );
    }
}

#endif /* STARMATH_INCLUDE_PIPELINE_VIEW_WRITE_H_ */
