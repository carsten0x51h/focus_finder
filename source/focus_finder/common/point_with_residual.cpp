#include <ostream>

#include "include/point_with_residual.h"

PointWithResidualT::PointWithResidualT(const PointFT & point, float residual) : point(point), residual(residual) {

}

std::ostream & operator<<(std::ostream & os,
		const PointWithResidualT & pointWithResidual) {
	static const char * prefix = "   ";

	os << prefix << "** PointWithResidual **" << std::endl;
	os << prefix << " > Point (x,y)=(" << pointWithResidual.point << ") - residual=" << pointWithResidual.residual << std::endl;

	return os;
}
