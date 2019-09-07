/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CENTROID_VIEW_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CENTROID_VIEW_H_

#include "centroid.h"

class CentroidViewT {
public:
	static cimg_library::CImg<unsigned char> genView(const CentroidT & centroid, float scaleFactor = defaultScaleFactor);

	static const float defaultScaleFactor;
};




#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CENTROID_VIEW_H_ */


