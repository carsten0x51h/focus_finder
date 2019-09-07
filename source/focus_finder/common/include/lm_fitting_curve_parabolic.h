#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_CURVE_PARABOLIC_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_CURVE_PARABOLIC_H_

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "curve_fit_exception.h"
#include "gsl_multi_fit_parms.h"

#include "lm_fitting_curve.h"

#include "enum_helper.h"


class LmFittingCurveParabolicT : public LmFittingCurveT {

public:
	struct IdxT {
			enum TypeE {
				A_IDX = 0, B_IDX, C_IDX, _Count
			};

			static const char * asStr(const TypeE & inType) {
				switch (inType) {
				case A_IDX:
					return "A_IDX";
				case B_IDX:
					return "B_IDX";
				case C_IDX:
					return "C_IDX";
			default:
				return "<?>";
			}
		}
		MAC_AS_TYPE(Type, E, _Count);
	};



	std::string getName() const override;
	std::vector<std::string> getParmNames() const override;

    float fx(float x, const gsl_vector * curveParms) const override;

	void makeGuess(const GslMultiFitParmsT & inData, gsl_vector * guess) override;
	int gslFx(const gsl_vector * curveParms, const GslMultiFitParmsT * gslParms, gsl_vector * outResultVec) override;
	int gslDfx(const gsl_vector * x, const GslMultiFitParmsT * gslParms, gsl_matrix * J) override;
	int gslFdfx(const gsl_vector * x, const GslMultiFitParmsT * gslParms, gsl_vector * f, gsl_matrix * J) override;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_CURVE_PARABOLIC_H_*/
