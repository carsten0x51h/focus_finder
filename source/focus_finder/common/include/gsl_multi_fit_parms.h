#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_GSL_MULTI_FIT_PARMS_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_GSL_MULTI_FIT_PARMS_H_

#include <vector>
#include "point.h"

///////////////////////////////////////////////////////////////////////
// Helper classes
///////////////////////////////////////////////////////////////////////
struct GslMultiFitDataT {
  //GslMultiFitDataT() : y(0.0f), sigma(0.0f) {}
GslMultiFitDataT(float y, float sigma, PointFT pt) : y(y), sigma(sigma), pt(pt) { }
	float y;
	float sigma;
	PointFT pt;
};

typedef std::vector<GslMultiFitDataT> GslMultiFitParmsT;


#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_GSL_MULTI_FIT_PARMS_H_*/
