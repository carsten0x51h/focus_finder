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
0 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

#include <memory>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "include/image_writer.h"
#include "include/cimg_fits_io.h"


namespace starmath::io {

    /**
     * NOTE:
     * ends_with() is defined in C++20 and may be used here once all supported OS versions have C++20 support.
     *
     * See https://en.cppreference.com/w/cpp/string/basic_string/ends_with
     *
     * @param filepath_lower
     * @return
     */
    bool is_fits(const std::string & filepath_lower) {
        return (boost::algorithm::ends_with(filepath_lower, "fit") || boost::algorithm::ends_with(filepath_lower, "fits"));
    }


    /**
     *
     * @param filepath_lower
     * @return
     */
    bool is_fits_gz(const std::string & filepath_lower) {
        return (boost::algorithm::ends_with(filepath_lower, "fit.gz") || boost::algorithm::ends_with(filepath_lower, "fits.gz"));
    }


    /**
     * TODO: What to specify? -> A concrete filename!
     * TODO: Change logic... check if directory exists..., check if file exists, is override set? -> Exception, or not...
     *
     * @param filepath
     */
    void check_filepath(const std::filesystem::path & filepath) {

    	TODO: Fixme.... wrong logic here...

        if (is_empty(filepath)) {
            throw ImageWriterExceptionT("Empty file path specified.");
        }
        else if (! is_regular_file(filepath) || is_directory(filepath)) {
            throw ImageWriterExceptionT("Specified file path is not a regular file.");
        }
        else if (! filepath.has_extension()) {
            throw ImageWriterExceptionT("Unable to determine file extension.");
        }
    }


    /**
     *
     * @param filepath
     * @param image
     */
    void write_fits(const std::string & filepath, const ImageT & img) {

        std::stringstream debugSs;

        try {
        	// NOTE: Throws FitsIOExceptionT
        	starmath::io::fits::write(img, filepath, & debugSs);
        } catch (FitsIOExceptionT &exc) {
            std::stringstream ss;
            ss << "Error writing image to '" << filepath << "'. FitsIO exception occurred: " << exc.what();
            ss << "Details: " << debugSs.str();

            throw ImageWriterExceptionT(ss.str());
        }
    }


    /**
     *
     */
    void write(const ImageT & img, const std::filesystem::path & filepath) {
		check_filepath(filepath);

        const std::string filepath_lower = boost::algorithm::to_lower_copy(filepath.string());

        if (is_fits(filepath_lower) || is_fits_gz(filepath_lower)) {
            write_fits(filepath.string(), img);
        }
        else {
        	img.save(filepath.string().c_str());
        }
	}

}
