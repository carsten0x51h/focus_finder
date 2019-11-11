#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_H_

#include "curve_function.h"
#include "curve_parms.h"
#include "point.h"
#include "exception.h"

DEF_Exception(CurveFunction);

class CurveFunctionT {
 protected:
  CurveParmsT mCurveParms;
    
 public:
  CurveFunctionT(const CurveParmsT & curveParms);
  const CurveParmsT & getCurveParms() const;
  virtual std::string getName() const = 0;
  virtual float f(float x) const = 0;
  virtual PointFT min() const = 0; // TODO: Is it a good idea to have a min() function in a generic CurveFunctionT?
  virtual PointFT max() const = 0; // TODO: Is it a good idea to have a max() function in a generic CurveFunctionT?
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CURVE_FUNCTION_H_*/
