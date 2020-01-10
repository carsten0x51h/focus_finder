/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c.schmitt51h@gmail.com>
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

#include "include/image_converter_16to8.h"
#include "include/image.h"
#include "include/logging.h"
#include "include/throw_if.h"

#include <limits>
#include <stdint.h>

#include <cmath>

ImageConverter16To8T::ImageConverter16To8T() {
	mLut.resize(std::numeric_limits < uint16_t > ::max() + 1);
}

void ImageConverter16To8T::setMapperFunction(
		std::shared_ptr<MapperFunctionT> mapperFunction) {
	LOG(debug) << "ImageConverter16To8T::setMapperFunction..." << std::endl;

	mMapperFunction = mapperFunction;

	mMapperFunction->setSrcRange(0, std::numeric_limits < uint16_t > ::max() + 1);
	mMapperFunction->setDestRange(0, std::numeric_limits<unsigned char>::max() + 1);

	updateLut();
}

std::shared_ptr<MapperFunctionT> ImageConverter16To8T::getMapperFunction() const {
	return mMapperFunction;
}

void ImageConverter16To8T::update() {
	updateLut();
}

void ImageConverter16To8T::convert(const cimg_library::CImg<uint16_t> & src,
		cimg_library::CImg<unsigned char> * dest) {

	LOG(debug) << "ImageConverter16To8T::convert..." << std::endl;

	THROW_IF(ImageConverter16To8, dest == nullptr, "dest expected to be set.");

	dest->assign(src.width(), src.height());

	cimg_forXY(src, x, y)
	{
		(*dest)(x, y) = mLut[src(x, y)];
	}
}

void ImageConverter16To8T::updateLut() {
	LOG(debug) << "ImageConverter16To8T::updateLut..." << std::endl;

	if (mMapperFunction == nullptr) {
		return;
	}

	for (size_t idx = 0; idx < mLut.size(); ++idx) {
		mLut[idx] = (unsigned char) mMapperFunction->f(idx);
	}
}
