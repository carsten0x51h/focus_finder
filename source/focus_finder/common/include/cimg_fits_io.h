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

#ifndef STARMATH_CIMG_FITS_IO_H
#define STARMATH_CIMG_FITS_IO_H STARMATH_CIMG_FITS_IO_H

#include <memory>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "exception.h"
#include "image.h"


namespace starmath::io::fits {

	DEF_Exception(FitsIO);


    /**
     * NOTE:
     * ends_with() is defined in C++20 and may be used here once all supported OS versions have C++20 support.
     *
     * See https://en.cppreference.com/w/cpp/string/basic_string/ends_with
     *
     * @param filepath_lower
     * @return
     */
    bool is_fits(const std::string & filepath_lower);


    /**
     *
     * @param filepath_lower
     * @return
     */
    bool is_fits_gz(const std::string & filepath_lower);


    /**
     * CCfits helper function
     * See http://heasarc.gsfc.nasa.gov/fitsio/ccfits/html/cookbook.html
     *
     * TODO: Add/adapt unit tests? -> Load, save and load -> compare...
     * TODO: Improve error handling?
     */
    std::shared_ptr<ImageT>
    read(const std::string &inFilename, std::stringstream *ss = nullptr);

    /**
     * CCfits helper function
     * See http://heasarc.gsfc.nasa.gov/fitsio/ccfits/html/cookbook.html
     *
     * TODO: Add/adapt unit tests? -> Load, save and load -> compare...
     * TODO: Improve error handling?
     * TODO: Add write(const std::shared_ptr<ImageT> ...) ?
     *
     */
    void
	write(const ImageT &inImg, const std::string &inFilename, std::stringstream *ss = nullptr);

}; // end namespace starmath::io::fits

#endif // STARMATH_CIMG_FITS_IO_H

