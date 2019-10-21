#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_H_

#include "curve_function.h"
#include "curve_parms.h"

class CurveFunctionT {
 protected:
  CurveParmsT mCurveParms;
    
 public:
   CurveFunctionT(const CurveParmsT & curveParms);
   const CurveParmsT & getCurveParms() const;
   virtual std::string getName() const = 0;
   virtual float f(float x) const = 0;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_H_*/
