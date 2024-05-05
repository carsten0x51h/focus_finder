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

#include "include/image_writer.h"
#include "include/cimg_fits_io.h"


namespace starmath::io {


    /**
     * Check that the specified path is not an existing directory, that the
     * specified name has an extension, and that an exception is thrown in case
     * the file already exists and override is set to false.
     *
     * @param filepath  File path where the image should be stored to.
     * @param override  Set to true allows overriding an existing file.
     */
    void check_filepath(const std::filesystem::path & filepath, bool override) {

    	if (is_directory(filepath)) {
    		throw ImageWriterExceptionT("Specified file path is a directory.");
    	}
    	else if (! filepath.has_extension()) {
    		throw ImageWriterExceptionT("Unable to determine file extension.");
    	}
    	else if (! std::filesystem::exists(filepath.parent_path())) {
    		// TODO: Maybe sub-folders are automatically created and this check is not required.
    		throw ImageWriterExceptionT("Specified directory does not exist.");
    	}
    	else if (is_regular_file(filepath) && ! override) {
    		std::stringstream ss;
    		ss << "File '" << filepath << "' already exists and override is disabled.";
    		throw ImageWriterExceptionT(ss.str());
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
        } catch (starmath::io::fits::FitsIOExceptionT &exc) {
            std::stringstream ss;
            ss << "Error writing image to '" << filepath << "'. FitsIO exception occurred: " << exc.what();
            ss << "Details: " << debugSs.str();

            throw ImageWriterExceptionT(ss.str());
        }
    }


    /**
     *
     */
    void write(const ImageT & img, const std::filesystem::path & filepath, bool override) {
		check_filepath(filepath, override);

        const std::string filepath_lower = boost::algorithm::to_lower_copy(filepath.string());

        if (starmath::io::fits::is_fits(filepath_lower) || starmath::io::fits::is_fits_gz(filepath_lower)) {
            write_fits(filepath.string(), img);
        }
        else {
        	img.save(filepath.string().c_str());
        }
	}

}
