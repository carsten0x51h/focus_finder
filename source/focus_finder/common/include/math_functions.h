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
