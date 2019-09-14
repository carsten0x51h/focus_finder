/*
 * image.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_IMAGE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_IMAGE_H_

// We do not want X11 dependency
#define cimg_display 0

#include <CImg.h>

// If X11 is used, the following things need to be undefined because of a conflict with definitions in QT5.
// https://stackoverflow.com/questions/22476110/c-compiling-error-including-x11-x-h-x11-xlib-h
//#undef Bool
//#undef CursorShape
//#undef Expose
//#undef KeyPress
//#undef KeyRelease
//#undef FocusIn
//#undef FocusOut
//#undef FontChange
//#undef None
//#undef Status
//#undef Unsorted

/**
 *
 */
// TODO: Introduce??
//Image8T
//Image16T
//ImageFT
// TODO: Introduce bit depth? -> probelmatic for float type...
using ImageT = cimg_library::CImg<float>;


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_IMAGE_H_ */


