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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FLOATING_POINT_EQUALITY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FLOATING_POINT_EQUALITY_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FLOATING_POINT_EQUALITY_H_

#pragma once    

#include <limits>
#include <cmath>  /* for std::abs(double) */

#include "image.h"


// https://stackoverflow.com/questions/19837576/comparing-floating-point-number-to-zero
// see Knuth section 4.2.2 pages 217-218
template<typename T>
bool isAlmostEqual(T x, T y) {
    return std::abs(x - y) <= std::numeric_limits<T>::epsilon() * std::abs(x);
}


/**
 * TODO: Passing a fixed limit like below is bad practice...
 *       -> A lambda function could be used to implement s limit like
 *          std::numeric_limits<typename ImageT::value_type>::epsilon() * std::abs(img1(x,y));
 */
static bool isAlmostEqual(const ImageT & img1, const ImageT & img2, double acceptable_tolerance = 0.00001) {
  bool same_width = (img1.width() == img2.width());
  bool same_height = (img1.height() == img2.height());
  bool same_size = same_width && same_height;

  if (!same_size) {
    return false;
  }

  ImageT sub = img1 - img2;
  
  cimg_forXY(sub, x, y) {
    bool considerEqual = (std::abs(sub(x,y)) <= acceptable_tolerance);

    if (! considerEqual) {
      return false;
    }
  }

  return true;
}




#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FLOATING_POINT_EQUALITY_H_*/
