/*
 * focus.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SNR_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SNR_H_

#include "image.h"

// TODO: We may extend this class so that it can also be instantiated to store the diffeent
//       values which were required on the way to the actual SNR calculation as well as the
//       corresponding image.

class SnrT {
private:
	SnrT() {}; // private for now

public:
	/**
	 * There are some different ways to calculate the "SNR".
	 * The following document explains in detail of how the SNR can be
	 * calculated IF an the noise can be measured somewhere in the image
	 * - e.g. in the background or at least is known:
	 *
	 * https://www2.ph.ed.ac.uk/~wjh/teaching/dia/documents/noise.pdf
	 *
	 * The CImg library provides a variance estimator function (variance_noise()).
	 * With this function a more or less typical noise is simulated
	 * so that a more complicated background extraction and noise measurement
	 * does not need to be implemented here. For the purpose of recognzing if
	 * a star is within the "selection window" or not it is sufficient.
	 *
	 * Also see
	 * - https://en.wikipedia.org/wiki/Signal-to-noise_ratio_(imaging)
	 * - https://www.researchgate.net/post/How_is_SNR_calculated_in_images
	 * - https://www.dspguide.com/ch25/3.htm
	 * - https://www2.ph.ed.ac.uk/~wjh/teaching/dia/documents/noise.pdf
	 * - https://nptel.ac.in/courses/117104069/chapter_1/1_11.html
	 * - https://nptel.ac.in/courses/117104069/chapter_1/1_12.html
	 * - https://kogs-www.informatik.uni-hamburg.de/~neumann/BV-WS-2010/Folien/BV-4-10.pdf
	 */
	static double calculate(const ImageT & image);
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SNR_H_ */
