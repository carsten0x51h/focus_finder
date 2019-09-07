#include "include/snr.h"
#include "include/logging.h"

#include <cmath>


double SnrT::calculate(const ImageT & image) {
	// 0 = Calc. variance  as "second moment" according to
	// https://kogs-www.informatik.uni-hamburg.de/~neumann/BV-WS-2010/Folien/BV-4-10.pdf
	double varianceOfImage = image.variance(0);

	double estimatedVarianceOfNoise = image.variance_noise(0);
	double q = varianceOfImage / estimatedVarianceOfNoise;
	double qClip = (q > 1 ? q : 1);

	// The simulated variance of the noise will be different from the noise in the real image.
	// Therefore it can happen that q becomes < 1. If that happens it should be limited to 1.
	double snr = std::sqrt(qClip - 1);

	LOG(debug) << "SnrT::calculate ...varianceOfImage: " << varianceOfImage
			   << ", estimatedVarianceOfNoise: " << estimatedVarianceOfNoise
			   << ", snr: " << snr << std::endl;

	return snr;
}
