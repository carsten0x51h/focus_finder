#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_FITTING_CURVE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_FITTING_CURVE_H_

#include <string>
#include <vector>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "gsl_multi_fit_parms.h"

#include "curve_parms.h"

class LmFittingCurveT {
public:
	virtual std::string getName() const = 0;


	//virtual std::vector<std::string> getParmNames() const = 0;
	template<typename Idx>
	std::vector<std::string>
	getParmNames() const {
		std::vector<std::string> parmNames(Idx::_Count);

		for (size_t idx = 0; idx < Idx::_Count; ++idx) {
			typename Idx::TypeE t = static_cast<typename Idx::TypeE>(idx);
			parmNames[idx] = Idx::asStr(t);
		}

		return parmNames;
	}

	// Those static wrappers are required since a pointer to them is passed to the C API of GSL.
	static int gslFx(const gsl_vector * curveParms, void * parms, gsl_vector * outResultVec);
	static int gslDfx(const gsl_vector * x, void * parms, gsl_matrix * J);
	static int gslFdfx(const gsl_vector * x, void * parms, gsl_vector * f, gsl_matrix * J);

	virtual std::vector<std::string> getParmNames() const = 0;


	float fx(float x, const CurveParmsT & curveParms) const;
	virtual float fx(float x, const gsl_vector * curveParms) const = 0;

	virtual void makeGuess(const GslMultiFitParmsT & inData, gsl_vector * guess) = 0;
	virtual int gslFx(const gsl_vector * x, const GslMultiFitParmsT * gslParams, gsl_vector * outResultVec) = 0;
	virtual int gslDfx(const gsl_vector * x, const GslMultiFitParmsT * gslParams, gsl_matrix * J) = 0;
	virtual int gslFdfx(const gsl_vector * x, const GslMultiFitParmsT * gslParams, gsl_vector * f, gsl_matrix * J) = 0;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_FITTING_CURVE_H_*/
