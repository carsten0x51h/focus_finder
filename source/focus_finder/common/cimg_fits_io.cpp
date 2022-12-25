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

#include "include/cimg_fits_io.h"
#include "include/throw_if.h"

#include <CCfits/CCfits>
#include <memory>
#include <sstream>

// TODO: As template...???
// TODO: Improve error handling...
void CImgFitsIOHelperT::readFits(ImageT *outImg,
                                 const std::string &inFilename, long *outBitPix,
                                 std::stringstream *ss) {

    THROW_IF(FitsIO, outImg == nullptr, "outImg expected to be set.")

    CCfits::FITS::setVerboseMode(ss != nullptr);

    try {
        std::unique_ptr<CCfits::FITS
        > pInfile(new CCfits::FITS(inFilename, CCfits::Read, true));
        CCfits::PHDU &image = pInfile->pHDU();

        if (outBitPix != nullptr) {
            *outBitPix = image.bitpix();
        }

        // read all user-specific, coordinate, and checksum keys in the image
        image.readAllKeys();

        if (ss != nullptr) {
            *ss << image << std::endl;
        }

        // Set image dimensions
        outImg->resize((int) image.axis(0) /*x*/, (int) image.axis(1) /*y*/, 1/*z - TODO: HACK*/,
                       1 /*1 color - TODO: HACK*/);

        // HACK / FIXME: At this point we assume that there is only 1 layer!
        std::valarray<typename ImageT::value_type> imgData;
        image.read(imgData);

        // For now we create a copy... maybe there is a better way to directly read data into CImg, later...
        cimg_forXY(*outImg, x, y) {
                // TODO: Should this be parameterized? Or is it possible to find out automatically?
                // Correct, when reading old, existing FITS files
                // NOTE: ImageJ and Gimp both work this way for normal files.
                //       -> For INDI/BLOB there must be a different solution.
                (*outImg)(x, outImg->height() - 1 - y) = imgData[outImg->offset(x, y)];

                // Correct when reading the image directly after storing the BLOB file with INDI.
                //(*outImg)(x, y) = imgData[outImg->offset(x, y)];
            }

    } catch (CCfits::FitsException &exc) {
        throw FitsIOExceptionT(exc.message());
    }
}

// TODO: As template...
// TODO: more generic..?
// TODO: error handling?
void CImgFitsIOHelperT::writeFits(const ImageT &inImg,
                                  const std::string &inFilename, std::stringstream *ss) {

    // TODO: Is it possible to pass a stream?
    CCfits::FITS::setVerboseMode(ss != nullptr);

    try {
        long naxis = 2;
        long naxes[2] = {inImg.width(), inImg.height()};

        std::unique_ptr<CCfits::FITS> pFits;

        pFits = std::make_unique<CCfits::FITS>(
                std::string("!") + inFilename, USHORT_IMG,
                                 naxis, naxes);

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

