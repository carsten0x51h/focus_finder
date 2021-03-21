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

#include "include/star_cluster_algorithm.h"
#include "include/logging.h"


StarClusterAlgorithmT::StarClusterAlgorithmT(size_t clusterRadius) {
    initOffsetPattern((int) clusterRadius);
}

void StarClusterAlgorithmT::initOffsetPattern(int n) {

    mOffsets.reserve(n * n - 1);

    for (int i = -n; i <= n; ++i) {
        for (int j = -n; j <= n; ++j) {
            // Only add if not 0,0
            if (i != 0 || j != 0) {
                mOffsets.push_back(PixelPosT(i, j));

                LOG(debug) << "Adding offset (" << i << ", " << j << ")." << std::endl;
            }
        }
    }
}


void
StarClusterAlgorithmT::getAndRemoveNeighbours(const PixelPosT &inCurPixelPos, PixelPosSetT *inoutWhitePixels,
                                              StarClusterT *inoutPixelsToBeProcessed,
                                              StarClusterT *outPixelCluster) {
    // TODO: Does not work!

    for (const PixelPosT &offset : mOffsets) {
        PixelPosT curPixPos(inCurPixelPos.x() + offset.x(), inCurPixelPos.y() + offset.y());

        PixelPosSetT::iterator itPixPos = inoutWhitePixels->find(curPixPos);

        if (itPixPos != inoutWhitePixels->end()) {
            const PixelPosT &curWhitePixPos = *itPixPos;
            inoutPixelsToBeProcessed->push_back(curWhitePixPos);
            outPixelCluster->push_back(curWhitePixPos);
            inoutWhitePixels->erase(itPixPos); // Remove white pixel from "white set" since it has been processed, now
        }
    }
}


std::list<StarClusterT> StarClusterAlgorithmT::cluster(const ImageT &inImg) {
    std::list<StarClusterT> recognizedClusters;
    PixelPosSetT whitePixels;

    cimg_forXY(inImg, x, y) {
            if (inImg(x, y) != 0) {
                whitePixels.insert(whitePixels.end(), PixelPosT(x, y));
            }
        }

    // Iterate over white pixels as long as set is not empty
    while (!whitePixels.empty()) {
        StarClusterT pixelCluster;
        StarClusterT pixelsToBeProcessed;

        PixelPosSetT::iterator itWhitePixPos = whitePixels.begin();

        pixelsToBeProcessed.push_back(*itWhitePixPos);
        whitePixels.erase(itWhitePixPos);

        while (!pixelsToBeProcessed.empty()) {
            PixelPosT curPixelPos = pixelsToBeProcessed.front();

            getAndRemoveNeighbours(curPixelPos, &whitePixels, &pixelsToBeProcessed, &pixelCluster);
            pixelsToBeProcessed.pop_front();
        }

        // Finally, append the cluster
        recognizedClusters.push_back(pixelCluster);
    }

    return recognizedClusters;
}
