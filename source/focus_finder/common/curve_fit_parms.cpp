#include <ostream>

#include "include/curve_fit_parms.h"

// TODO: Does it make sense to define default values for epsrel and absrel? What are common values?
CurveFitParmsT::CurveFitParmsT(float epsrel, float epsabs,
			       std::size_t numMaxIterations = 100, float outlierBoundaryFactor = 1.5F,
		float maxAcceptedOutliersPerc = 10.0F) :

		epsrel(epsrel), epsabs(epsabs), numMaxIterations(numMaxIterations), outlierBoundaryFactor(
				outlierBoundaryFactor), maxAcceptedOutliersPerc(
				maxAcceptedOutliersPerc) {

}

CurveFitParmsT::CurveFitParmsT(float epsrel, float epsabs,
		std::size_t numMaxIterations = 100) :
		CurveFitParmsT(epsrel, epsabs, numMaxIterations,
				-1 /*disable outlier detection*/, 0.0F) {

}

std::ostream & operator<<(std::ostream & os,
		const CurveFitParmsT & curveFitParms) {
	static const char * prefix = "   > ";

	os << "** CurveFitParms **" << std::endl;
	os << prefix << "EpsRel: " << curveFitParms.epsrel << std::endl
	   << prefix << "EpsAbs: " << curveFitParms.epsabs << std::endl
	   << prefix << "#MaxIterations: " << curveFitParms.numMaxIterations << std::endl
	   << prefix << "outlierBoundaryFactor: " << curveFitParms.outlierBoundaryFactor << std::endl
	   << prefix << "maxAcceptedOutliersPerc: " << curveFitParms.maxAcceptedOutliersPerc << std::endl;

	return os;
}
