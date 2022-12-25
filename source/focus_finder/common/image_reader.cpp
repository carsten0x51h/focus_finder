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

namespace fs = std::filesystem;
namespace ba = boost::algorithm;

namespace starmath::io {

    /**
     *
     *
     * NOTE: ends_with() is defined in C++20 and may be used here once all supported OS versions have C++20 support.
     *       See https://en.cppreference.com/w/cpp/string/basic_string/ends_with
     *
     * @param filepath_lower
     * @return
     */
    bool is_fits(const std::string & filepath_lower) {
        return (ba::ends_with(filepath_lower, "fit") || ba::ends_with(filepath_lower, "fits"));
    }


    /**
     *
     * @param filepath_lower
     * @return
     */
    bool is_fits_gz(const std::string & filepath_lower) {
        return (ba::ends_with(filepath_lower, "fit.gz") || ba::ends_with(filepath_lower, "fits.gz"));
    }


    /**
     *
     * @param filepath
     */
    void check_filepath(const fs::path & filepath) {
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
        auto new_image = std::make_shared<ImageT>();

        std::stringstream debugSs;
        long bitPix; // TODO: Add to CImg somehow? Or is it contained already?

        try {
            // NOTE: Throws FitsIOExceptionT
            CImgFitsIOHelperT::readFits(new_image.get(), filepath, & bitPix, & debugSs);
        } catch (FitsIOExceptionT &exc) {
            std::stringstream ss;
            ss << "FitsIO exception occurred: " << exc.what();
            throw ImageReaderExceptionT(ss.str());
        }

        return new_image;
    }

    /**
     *
     * @param filepath
     * @return
     */
    std::shared_ptr<ImageT> read(const fs::path & filepath) {

        check_filepath(filepath);

        const std::string filepath_lower = boost::algorithm::to_lower_copy(filepath.string());

        // TODO: This code may be moved to a sep. "factory", later.
        if (is_fits(filepath_lower) || is_fits_gz(filepath_lower)) {

            return read_fits(filepath.string());
        }
        else {
            return std::make_shared<ImageT>(filepath.string().c_str());
        }
    }
}
