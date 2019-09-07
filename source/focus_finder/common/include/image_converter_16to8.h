/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_IMAGE_CONVERTER_16TO8_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_IMAGE_CONVERTER_16TO8_H_

#include <memory>
#include <vector>
#include <stdint.h>

#include "image.h"
#include "mapper_function.h"
#include "exception.h"

DEF_Exception(ImageConverter16To8);

class ImageConverter16To8T {
public:
	ImageConverter16To8T();
	void setMapperFunction(std::shared_ptr<MapperFunctionT> mapperFunction);
	std::shared_ptr<MapperFunctionT> getMapperFunction() const;
	void update();

	/**
	 * 16 bit -> 8 bit
	 *
	 * TODO: RGB?!
	 * TODO: Use Image8T, Image16T,....
	 * TODO: Use mov semantics to return (move) result image?
	 */
	void convert(const cimg_library::CImg<uint16_t> & src, cimg_library::CImg<unsigned char> * dest);


private:
	void updateLut();

	std::shared_ptr<MapperFunctionT> mMapperFunction;
	std::vector<unsigned char> mLut;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_IMAGE_CONVERTER_16TO8_H_ */

