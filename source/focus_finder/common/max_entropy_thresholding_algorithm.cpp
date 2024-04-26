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

#include <cmath>
#include <vector>

#include "include/image.h"
#include "include/logging.h"
#include "include/max_entropy_thresholding_algorithm.h"
#include "include/floating_point_equality.h"

std::string MaxEntropyThresholdingAlgorithmT::getName() const { return "MaxEntropyThresholdingAlgorithmT"; }

// TODO: bitDepth is not required..?! -> remove it...
// TODO: Supply number of bins at parameter / template parameter?
// TODO: Supply image type also as template parameter?
float MaxEntropyThresholdingAlgorithmT::calc(const ImageT &inImg, long bitDepth) const {

    LOG(debug) << "MaxEntropyThresholdingAlgorithmT::calc..." << std::endl;

    if (inImg.width() <= 0 || inImg.height() <= 0) {
        throw ThresholdingExceptionT("No image supplied.");
    }

    size_t numBins = NUM_BINS;
    std::vector<float> hist(numBins, 0);

    float max;
    float min = inImg.min_max(max);

    LOG(debug) << "MaxEntropyThresholdingAlgorithmT::calc... bitDepth=" << bitDepth
               << ", numBins=" << numBins << ", min=" << min << ", max=" << max
               << ", img-dim (w x h)=" << inImg.width() << " x " << inImg.height() << std::endl;

    /**
     * IMPORTANT / IDEA: "Shrink" /map the float image pixel values to 256 possible brightness levels (i.e. 256 histogram bins).
     * The reason is that the performance of the "max entropy algorithm" strongly depends on the histogram size / number of bins.
     * Note that the threshold which will be calculated based on this histogram will be the correct one for this "shrinked"
     * histogram. In order to get the threshold for the initial float image, this transformation needs to be reverted
     * later (see comment below).
     */
    cimg_forXY(inImg, x, y) {
            int idx = (int) ((float) (numBins - 1) * (inImg(x, y) - min) / (max - min));
            ++hist[idx];
        }

    // Normalize histogram (sum of all is 1)
    float sum = (float) inImg.width() * (float) inImg.height();

    std::vector<float> normHist(hist);
    for (auto it = normHist.begin(); it != normHist.end(); ++it) {
        *it = *it / sum;
    }

    // Calculate accumulated histograms
    std::vector<float> accumulatedHistBlack(numBins, 0);
    std::vector<float> accumulatedHistWhite(numBins, 0);

    float accumHistSum = 0.0F;
    for (size_t idx = 0; idx < numBins; ++idx) {
        accumHistSum += normHist[idx];

        accumulatedHistBlack[idx] = accumHistSum;
        accumulatedHistWhite[idx] = 1.0F - accumHistSum;
    }

    // Find first index of element not 0 in black distribution
    size_t first_bin_idx = 0;

    for (size_t idx = 0; idx < numBins; ++idx) {
      if (!isAlmostEqual(accumulatedHistBlack[idx], 0.0F)) {
            first_bin_idx = idx;
            break;
        }
    }

    // Find last index of element not 0 in white distribution
    size_t last_bin_idx = numBins;

    for (size_t idx = numBins - 1; idx >= first_bin_idx; --idx) {
        if (!isAlmostEqual(accumulatedHistWhite[idx], 0.0F)) {
            last_bin_idx = idx;
            break;
        }
    }

    LOG(debug) << "MaxEntropyThresholdingAlgorithmT::calc... first_bin_idx: " << first_bin_idx << ", last_bin_idx: "
               << last_bin_idx << std::endl;


    float threshold = 0;
    float max_ent = 0;
    float ent_back;
    float ent_obj;
    float tot_ent;

    for (size_t idx = first_bin_idx; idx < last_bin_idx; ++idx) {

        /* Entropy of the background pixels */
        ent_back = 0.0;

        for (size_t ih = 0; ih <= idx; ih++) {
            if (!isAlmostEqual(normHist[ih], 0.0F)) {
                float c = normHist[ih] / accumulatedHistBlack[idx];
                ent_back -= c * std::log(c);
            }
        }

        ent_obj = 0.0;

        for (size_t ih = idx + 1; ih < numBins; ih++) {
            if (!isAlmostEqual(normHist[ih], 0.0F)) {
                float c = normHist[ih] / accumulatedHistWhite[idx];
                ent_obj -= c * std::log(c);
            }
        }

        /* Total entropy */
        tot_ent = ent_back + ent_obj;

        if (max_ent < tot_ent) {
            max_ent = tot_ent;
            threshold = (float) idx;
        }
    }

    /**
     * IMPORTANT: The histogram was "shrinked" to 256 values, i.e. float pixel value range was mapped to 256 brightness values.
     * This "shrinking" step needs to be reverted so that the calculated threshold matches the original float image.
     */
    float th2 = min + (threshold / (float) numBins) * (max - min);

    LOG(debug) << "MaxEntropyThresholdingAlgorithmT::calc...threshold: " << threshold << ", th2: " << th2 << std::endl;

    return th2;
}
