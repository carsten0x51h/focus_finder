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

#include "include/logging.h"
#include "include/otsu_thresholding_algorithm.h"
#include "include/floating_point_equality.h"

std::string OtsuThresholdingAlgorithmT::getName() const { return "OtsuThresholdingAlgorithmT"; }

float OtsuThresholdingAlgorithmT::calc(const ImageT &inImg, long bitDepth) const {

    LOG(debug) << "OtsuThresholdingAlgorithmT::calc..." << std::endl;

    size_t numBuckets = std::pow(2.0F, bitDepth);
    std::vector<float> hist(numBuckets, 0.0F);

    float numPixels = (float) inImg.width() * (float) inImg.height();
    float sumB = 0.0F;
    float wB = 0.0F;
    float max = 0.0F;
    float threshold1 = 0.0F;
    float threshold2 = 0.0F;

    LOG(debug) << "OtsuThresholdingAlgorithmT::calc - numBuckets: " << numBuckets << ", numPixels: " << numPixels
               << std::endl;

    // Calculate histogram - for some reason inImg.get_histogram() behaves different than expected.
    cimg_forXY(inImg, x, y) {
            int value = inImg(x, y);
            ++hist[value];
        }

    float sum = 0;
    for (size_t pos = 0; pos < numBuckets; ++pos) {
        sum += pos * hist[pos];
    }


    for (size_t i = 0; i < numBuckets; ++i) {
        wB += hist[i];

        if (isAlmostEqual(wB, 0.0F)) { continue; }

        float wF = numPixels - wB;

        if (isAlmostEqual(wF, 0.0F)) { break; }

        sumB += i * hist[i];

        float mF = (sum - sumB) / wF;
        float mB = sumB / wB;
        float diff = mB - mF;
        float bw = wB * wF * std::pow(diff, 2.0F);

        if (bw >= max) {
            threshold1 = i;
            if (bw > max) {
                threshold2 = i;
            }
            max = bw;
        }
    } // end loop

    LOG(debug) << "OtsuThresholdingAlgorithmT::calc - threshold1: " << threshold1 << ", threshold2: " << threshold2
               << std::endl;

    return (threshold1 + threshold2) / 2.0F;
}
