/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c [at] lost-infinity.com>
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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_OTSU_THRESHOLDING_ALGORITHM_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_OTSU_THRESHOLDING_ALGORITHM_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_OTSU_THRESHOLDING_ALGORITHM_H_

#include "thresholding_algorithm.h"
#include "image.h"

/**
 * Otsu’s method assumes that our image contains two classes of pixels: the background and
 * the foreground. Furthermore, Otsu’s method makes the assumption that the grayscale
 * histogram of our pixel intensities of our image is bi-modal, which simply means that
 * the histogram is two peaks.
 *
 * Otsu's thresholding method involves iterating through all the possible threshold
 * values and calculating a measure of spread for the pixel levels each side of the
 * threshold, i.e. the pixels that either fall in foreground or background. The aim
 * is to find the threshold value where the sum of foreground and background spreads
 * is at its minimum.
 *
 * It’s also important to note that Otsu’s method is an example of global thresholding
 * — implying that a single thresholding value is computed for the entire image.
 *
 * This threshold is determined by minimizing intra-class intensity variance, or
 * equivalently, by maximizing inter-class variance. The algorithm exhaustively
 * searches for the threshold that minimizes the intra-class variance, defined
 * as a weighted sum of variances of the two classes
 *
 * See https://rndayala.wordpress.com/2019/11/13/image-processing-thresholding/
 * See http://www.labbookpages.co.uk/software/imgProc/otsuThreshold.html
 */
class OtsuThresholdingAlgorithmT : public ThresholdingAlgorithmT {
public:
    [[nodiscard]] std::string getName() const override;

    [[nodiscard]] float calc(const ImageT &inImg, long bitDepth) const override;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_OTSU_THRESHOLDING_ALGORITHM_H_*/
