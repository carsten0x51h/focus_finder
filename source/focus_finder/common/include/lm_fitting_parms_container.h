#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_PARMS_CONTAINER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_PARMS_CONTAINER_H_

//class LmFittingCurveT;

class LmFittingParmsContainerT {
public:
	LmFittingParmsContainerT(const GslMultiFitParmsT * inGslParms, LmFittingCurveT * inObj) : gslParms(inGslParms), obj(inObj) {}
	const GslMultiFitParmsT * gslParms;
	LmFittingCurveT * obj;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_PARMS_CONTAINER_H_*/
