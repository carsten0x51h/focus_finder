#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CURVE_FIT_PARMS_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CURVE_FIT_PARMS_H_


class CurveFitParmsT {
public:
	CurveFitParmsT(float epsrel, float epsabs, size_t numMaxIterations, float outlierBoundaryFactor, float maxAcceptedOutliersPerc);
	CurveFitParmsT(float epsrel, float epsabs, size_t numMaxIterations);

	float epsrel;
	float epsabs;
	size_t numMaxIterations;
	float outlierBoundaryFactor; // set to -1 to disable
	float maxAcceptedOutliersPerc;

    friend std::ostream & operator<<(std::ostream & os, const CurveFitParmsT & curveFitParms);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CURVE_FIT_PARMS_H_*/
