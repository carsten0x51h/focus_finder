#include "include/image_converter_16to8.h"
#include "include/image.h"
#include "include/logging.h"
#include "include/throw_if.h"

#include <limits>
#include <stdint.h>

#include <cmath>

ImageConverter16To8T::ImageConverter16To8T() {
	mLut.resize(std::numeric_limits < uint16_t > ::max());
}

void ImageConverter16To8T::setMapperFunction(
		std::shared_ptr<MapperFunctionT> mapperFunction) {
	LOG(debug) << "ImageConverter16To8T::setMapperFunction..." << std::endl;

	mMapperFunction = mapperFunction;

	mMapperFunction->setSrcRange(0, std::numeric_limits < uint16_t > ::max());
	mMapperFunction->setDestRange(0, std::numeric_limits<unsigned char>::max());

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
