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

#include <CCfits/CCfits>
#include <memory>

#include "include/cimg_fits_io.h"


namespace starmath::io::fits {

  /**
   * NOTE:
   * ends_with() is defined in C++20 and may be used here once all supported OS versions have C++20 support.
   *
   * See https://en.cppreference.com/w/cpp/string/basic_string/ends_with
   *
   */
    bool
	is_fits(const std::string & filepath_lower) {
        return (boost::algorithm::ends_with(filepath_lower, "fit") || boost::algorithm::ends_with(filepath_lower, "fits"));
    }

    bool
	is_fits_gz(const std::string & filepath_lower) {
        return (boost::algorithm::ends_with(filepath_lower, "fit.gz") || boost::algorithm::ends_with(filepath_lower, "fits.gz"));
    }


	std::shared_ptr<ImageT>
    read(const std::string &inFilename, std::stringstream *ss) {

		CCfits::FITS::setVerboseMode(ss != nullptr);

		try {
			// TODO: Memory leak? new....
			std::unique_ptr<CCfits::FITS
			> pInfile(new CCfits::FITS(inFilename, CCfits::Read, true));
			CCfits::PHDU &fitsImg = pInfile->pHDU();

			// read all user-specific, coordinate, and checksum keys in the image
			fitsImg.readAllKeys();

			if (ss != nullptr) {
				*ss << fitsImg << std::endl;
			}

			auto img = std::make_shared<ImageT>((int) fitsImg.axis(0), (int) fitsImg.axis(1));
			//TODO: img->setBitDepth(fitsImg.bitpix());


			// TODO: Put a check here:   fitsImg.bitpix() <= sizeof(ImageT)
			std::cerr << "fitsImg.bitpix(): " << fitsImg.bitpix() << std::endl;
			std::cerr << "sizeof(ImageT): " << 8*sizeof(typename ImageT::value_type) << std::endl;



			// HACK / FIXME: At this point we assume that there is only 1 layer!
			std::valarray<typename ImageT::value_type> imgData;
			fitsImg.read(imgData);

			// For now we create a copy... maybe there is a better way to directly read data into CImg, later...
			cimg_forXY(*img, x, y) {
					// TODO: Should this be parameterized? Or is it possible to find out automatically?
					// Correct, when reading old, existing FITS files
					// NOTE: ImageJ and Gimp both work this way for normal files.
					//       -> For INDI/BLOB there must be a different solution.
					(*img)(x, img->height() - 1 - y) = imgData[img->offset(x, y)];

					// Correct when reading the image directly after storing the BLOB file with INDI.
					//(*outImg)(x, y) = imgData[outImg->offset(x, y)];
				}

			return img;

		} catch (CCfits::FitsException &exc) {
			throw FitsIOExceptionT(exc.message());
		}

		return nullptr;
    }


    void
	write(const ImageT &inImg, const std::string &inFilename, bool override, std::stringstream *ss) {
        // TODO: Is it possible to pass a stream?
        CCfits::FITS::setVerboseMode(ss != nullptr);

        try {
            long naxis = 2;
            long naxes[2] = {inImg.width(), inImg.height()};

            std::unique_ptr<CCfits::FITS> pFits;

            // NOTE: The "!" should automatically override an existing file...
            //       See https://heasarc.gsfc.nasa.gov/fitsio/ccfits/html/writeimage.html
            std::string filepath = (override ? "!" : "") + inFilename;

            if (ss != nullptr) {
            	*ss << "starmath::io::fits::write() writes to file '" << filepath << "'." << std::endl;
            }

            pFits = std::make_unique<CCfits::FITS>(filepath,
												   USHORT_IMG,
												   naxis,
												   naxes);

            // NOTE: At this point we assume that there is only 1 layer.
            long nelements = std::accumulate(&naxes[0], &naxes[naxis], 1,
                                                 std::multiplies<>());

            std::valarray<typename ImageT::value_type> array(nelements);

            cimg_forXY(inImg, x, y) {
                    array[inImg.offset(x, y)] = inImg(x, inImg.height() - y - 1);
                }

            long fpixel(1);
            pFits->pHDU().write(fpixel, nelements, array);
        } catch (CCfits::FitsException &exc) {
            throw FitsIOExceptionT(exc.message());
        }
    }

}; // end namespace starmath::io::fits

