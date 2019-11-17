#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_CURVE_HYPERBOLIC_POS_ONLY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_CURVE_HYPERBOLIC_POS_ONLY_H_

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "curve_fit_exception.h"
#include "gsl_multi_fit_parms.h"

#include "lm_fitting_curve.h"

#include "enum_helper.h"


class LmFittingCurveHyperbolicPosOnlyT : public LmFittingCurveT {
private:
  float mA;
  float mB;

  float phi(float x, float a, float c) const;

public:
  LmFittingCurveHyperbolicPosOnlyT();
	// struct IdxT {
	// 		enum TypeE {
	// 			A_IDX = 0, B_IDX, C_IDX, D_IDX, _Count
	// 		};

	// 		static const char * asStr(const TypeE & inType) {
	// 			switch (inType) {
	// 			case A_IDX:
	// 				return "A_IDX";
	// 			case B_IDX:
	// 				return "B_IDX";
	// 			case C_IDX:
	// 				return "C_IDX";
	// 			case D_IDX:
	// 				return "D_IDX";
	// 		default:
	// 			return "<?>";
	// 		}
	// 	}
	// 	MAC_AS_TYPE(Type, E, _Count);
	// };



	std::string getName() const override;
	std::vector<std::string> getParmNames() const override;

  float fx(float x, const gsl_vector * curveParms) const override;

	void makeGuess(const GslMultiFitParmsT & inData, gsl_vector * guess) override;
	int gslFx(const gsl_vector * curveParms, const GslMultiFitParmsT * gslParms, gsl_vector * outResultVec) override;
	int gslDfx(const gsl_vector * x, const GslMultiFitParmsT * gslParms, gsl_matrix * J) override;
	int gslFdfx(const gsl_vector * x, const GslMultiFitParmsT * gslParms, gsl_vector * f, gsl_matrix * J) override;

  
  void setA(float a);
  void setB(float b);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_CURVE_HYPERBOLIC_H_*/
