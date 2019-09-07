#include <gsl/gsl_vector.h>

#include "include/lm_fitting_curve.h"
#include "include/lm_fitting_parms_container.h"
#include "include/curve_parms.h"

// static!
int LmFittingCurveT::gslFx(const gsl_vector * curveParms, void * parms, gsl_vector * outResultVec) {
	LmFittingParmsContainerT * parmsContainer = static_cast<LmFittingParmsContainerT*>(parms);
	return parmsContainer->obj->gslFx(curveParms, parmsContainer->gslParms, outResultVec);
}

// static!
int LmFittingCurveT::gslDfx(const gsl_vector * x, void * parms, gsl_matrix * J) {
	LmFittingParmsContainerT * parmsContainer = static_cast<LmFittingParmsContainerT*>(parms);
	return parmsContainer->obj->gslDfx(x, parmsContainer->gslParms, J);
}

// static!
int LmFittingCurveT::gslFdfx(const gsl_vector * x, void * parms, gsl_vector * f, gsl_matrix * J) {
	LmFittingParmsContainerT * parmsContainer = static_cast<LmFittingParmsContainerT*>(parms);
	return parmsContainer->obj->gslFdfx(x, parmsContainer->gslParms, f, J);
}


float LmFittingCurveT::fx(float x, const CurveParmsT & curveParms) const {

	gsl_vector * gslParms = gsl_vector_alloc(curveParms.size());

	for (size_t idx = 0; idx < curveParms.size(); ++idx) {
		float value = curveParms.get(idx).getValue();
		gsl_vector_set(gslParms, idx, value);
	}

	float y = this->fx(x, gslParms);

	gsl_vector_free(gslParms);

	return y;
  }
