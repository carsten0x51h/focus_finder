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

