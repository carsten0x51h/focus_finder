#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_CURVE_GAUSSIAN_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_CURVE_GAUSSIAN_H_

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "curve_fit_exception.h"
#include "gsl_multi_fit_parms.h"

#include "lm_fitting_curve.h"

#include "enum_helper.h"



class LmFittingCurveGaussianT : public LmFittingCurveT {

public:
	struct IdxT {
			enum TypeE {
				B_IDX = 0, P_IDX, C_IDX, W_IDX, _Count
			};

			static const char * asStr(const TypeE & inType) {
				switch (inType) {
				case B_IDX:
					return "B_IDX";
				case P_IDX:
					return "P_IDX";
				case C_IDX:
					return "C_IDX";
				case W_IDX:
					return "W_IDX";
			default:
				return "<?>";
			}
		}
		MAC_AS_TYPE(Type, E, _Count);
	};


  float fx(float x, const gsl_vector * curveParms) const override;

	std::string getName() const override;
	std::vector<std::string> getParmNames() const override;

	void makeGuess(const GslMultiFitParmsT & inData, gsl_vector * guess) override;
	int gslFx(const gsl_vector * curveParms, const GslMultiFitParmsT * gslParms, gsl_vector * outResultVec) override;
	int gslDfx(const gsl_vector * x, const GslMultiFitParmsT * gslParms, gsl_matrix * J) override;
	int gslFdfx(const gsl_vector * x, const GslMultiFitParmsT * gslParms, gsl_vector * f, gsl_matrix * J) override;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_CURVE_HYPERBOLIC_H_*/
