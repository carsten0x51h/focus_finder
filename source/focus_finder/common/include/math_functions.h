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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_MATH_FUNCTIONS_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_MATH_FUNCTIONS_H_

#include <cmath>

class MathFunctionsT {
public:
  static float gaussian(float x, float b, float p, float c, float w) {
	float t = ((x - c) / w);
	t *= t;
	return b + p * std::exp(-0.5f * t);
  }

  /**
   * NOTE: Function only returns the positive part.
   */
  static float hyperbolic(float x, float a, float b, float c, float d) {
    	const float a2 = a * a;
	const float diff = x - c;
	const float diff2 = diff * diff;
	const float phi = std::sqrt(1.0 + (diff2 / a2));
	
	return b * phi + d;
  }

  /**
   * NOTE: Function only returns the positive part.
   */
  static float hyperbolic_inv(float x, float a, float b, float c, float d) {
    // TODO: Check that value under sqrt does not get < 0... currently we just return 0... is that ok?
    const float b2 = b * b;
    const float diff = x - d;
    const float diff2 = diff * diff;
    const float diff3 = (diff2 / b2) - 1.0;
    const float phi = (diff3 >= 0.0F ? std::sqrt(diff3) : 0.0F);
    
    return a * phi + c;
  }

  static float parabolic(float x, float a, float b, float c) {
    float x2 = x * x;
    return a * x2 + b * x + c;
  }

  
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_MATH_FUNCTIONS_H_*/
