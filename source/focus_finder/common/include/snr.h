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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SNR_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SNR_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SNR_H_

#include "image.h"

// TODO: We may extend this class so that it can also be instantiated to store the diffeent
//       values which were required on the way to the actual SNR calculation as well as the
//       corresponding image.

class SnrT {
private:
    SnrT() = default; // private for now

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
    static double calculate(const ImageT &image);
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SNR_H_ */
