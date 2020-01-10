/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c.schmitt51h@gmail.com>
 *
 *  More info on https://www.lost-infinity.com
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

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
