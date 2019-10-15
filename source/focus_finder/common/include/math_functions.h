#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_MATH_FUNCTIONS_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_MATH_FUNCTIONS_H_

class MathFunctionsT {
public:
  static float gaussian(float x, float b, float p, float c, float w) {
	float t = ((x - c) / w);
	t *= t;
	return b + p * exp(-0.5f * t);
  }

  static float hyperbolic(float x, float a, float b, float c, float d) {
    	const float a2 = a * a;
	const float diff = x - c;
	const float diff2 = diff * diff;
	const float phi = sqrt(1.0 + (diff2 / a2));
	
	return b * phi + d;
  }

  static float parabolic(float x, float a, float b, float c) {
    float x2 = x * x;
    return a * x2 + b * x + c;
  }

  
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_MATH_FUNCTIONS_H_*/
