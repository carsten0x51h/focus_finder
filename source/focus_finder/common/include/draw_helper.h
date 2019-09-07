/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DRAW_HELPER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DRAW_HELPER_H_

#include "image.h"
#include "point.h"

/**
 * Little draw helper function to draw a cross...
 */
class DrawHelperT {
public:
	static void drawCross(cimg_library::CImg<unsigned char> * inoutImg,
			float inX, float inY, const unsigned char * inColor,
			size_t inCrossSize = 3, float inScaleFactor = 1.0,
			size_t inOpacity = 1);

	static void drawCross(cimg_library::CImg<unsigned char> * inoutImg,
			const PointT<float> & inPos, const unsigned char * inColor,
			size_t inCrossSize = 3, float inScaleFactor = 1.0,
			size_t inOpacity = 1);
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DRAW_HELPER_H_ */
