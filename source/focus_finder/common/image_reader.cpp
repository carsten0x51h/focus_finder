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

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "include/image_reader.h"

#include "include/cimg_fits_io.h"


namespace starmath::io {

    /**
     *
     * @param filepath
     */
    void check_filepath(const std::filesystem::path & filepath) {
        if (is_empty(filepath)) {
            throw ImageReaderExceptionT("Empty file path specified.");
        }
        else if (! is_regular_file(filepath) || is_directory(filepath)) {
            throw ImageReaderExceptionT("Specified file path is not a regular file.");
        }
        else if (! filepath.has_extension()) {
            throw ImageReaderExceptionT("Unable to determine file extension.");
        }
    }


    /**
     *
     * @param filepath
     * @return
     */
    auto read_fits(const std::string & filepath) {

        std::stringstream debugSs;

        try {
            // NOTE: Throws FitsIOExceptionT
        	return starmath::io::fits::read(filepath, & debugSs);

        } catch (fits::FitsIOExceptionT &exc) {
            std::stringstream ss;
            ss << "FitsIO exception occurred: " << exc.what();
            ss << "Details: " << debugSs.str();
            throw ImageReaderExceptionT(ss.str());
        }
    }

    /**
     *
     */
    std::shared_ptr<ImageT> read(const std::filesystem::path & filepath) {

        check_filepath(filepath);

        const std::string filepath_lower = boost::algorithm::to_lower_copy(filepath.string());

        // TODO: This code may be moved to a sep. "factory", later.
        if (starmath::io::fits::is_fits(filepath_lower) || starmath::io::fits::is_fits_gz(filepath_lower)) {

            return read_fits(filepath.string());
        }
        else {
        	// TODO: Catch CImg exception and convert to ImageReaderExceptionT... -> Unit tests
            return std::make_shared<ImageT>(filepath.string().c_str());
        }
    }
}
