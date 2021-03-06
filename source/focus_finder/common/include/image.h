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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_IMAGE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_IMAGE_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_IMAGE_H_

// We do not want X11 dependency
#define cimg_display 0

// NOTE: In order to use TIFF, enable the following define. This also requires adding
//       ${TIFF_LIBRARIES} to the CMakeLists.txt file.
// See https://stackoverflow.com/questions/3021050/write-tiff-float-images-using-cimg
//#define cimg_use_tiff

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


