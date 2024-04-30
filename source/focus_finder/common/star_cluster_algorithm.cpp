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

#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/minmax.hpp>

#include "include/star_cluster_algorithm.h"
#include "include/logging.h"



RectT<int> PixelClusterT::getBounds() const {

	std::cerr << "PixelClusterT::getBounds()... 1" << std::endl;
	auto xCoordinateRange = mPixelPositions | ranges::views::transform([] (auto const pixelPos) { return pixelPos.x(); });
	std::cerr << "PixelClusterT::getBounds()... 2" << std::endl;
	auto yCoordinateRange = mPixelPositions | ranges::views::transform([] (auto const pixelPos) { return pixelPos.y(); });
	std::cerr << "PixelClusterT::getBounds()... 3" << std::endl;

	std::cerr << "#pixels: " << mPixelPositions.size() << std::endl;

	// Find top-left and bottom right pixel...
	auto [xmin, xmax] = ranges::minmax(xCoordinateRange);
	std::cerr << "PixelClusterT::getBounds()... xmin: " << xmin << ", xmax: " << xmax << std::endl;
	auto [ymin, ymax] = ranges::minmax(yCoordinateRange);
	std::cerr << "PixelClusterT::getBounds()... 5 ymin: " << ymin << ", ymax: " << ymax << std::endl;

	return RectT<int>(xmin, ymin, xmax - xmin + 1, ymax - ymin + 1);
}






StarClusterAlgorithmT::StarClusterAlgorithmT(size_t clusterRadius) {
    initOffsetPattern((int) clusterRadius);
}

void StarClusterAlgorithmT::initOffsetPattern(int n) {

    mOffsets.reserve(n * n - 1);

    for (int i = -n; i <= n; ++i) {
        for (int j = -n; j <= n; ++j) {
            mOffsets.emplace_back(i, j);

            LOG(debug) << "Adding offset (" << i << ", " << j << ")." << std::endl;
        }
    }
}


void
StarClusterAlgorithmT::getAndRemoveNeighbours(const PixelPosT &inCurPixelPos, PixelPosSetT *inoutWhitePixels,
		PixelPosListT *inoutPixelsToBeProcessed,
		PixelPosListT *outPixelCluster) {

    for (const PixelPosT &offset : mOffsets) {
        PixelPosT curPixPos(inCurPixelPos.x() + offset.x(), inCurPixelPos.y() + offset.y());

        auto itPixPos = inoutWhitePixels->find(curPixPos);

        if (itPixPos != inoutWhitePixels->end()) {
            const PixelPosT &curWhitePixPos = *itPixPos;
            inoutPixelsToBeProcessed->push_back(curWhitePixPos);
            outPixelCluster->push_back(curWhitePixPos);
            inoutWhitePixels->erase(itPixPos); // Remove white pixel from "white set" since it has been processed, now
        }
    }
}


std::list<PixelClusterT> StarClusterAlgorithmT::cluster(const ImageT &inImg) {

    std::list<PixelClusterT> recognizedClusters;
    PixelPosSetT whitePixels;

    cimg_forXY(inImg, x, y) {
            if (inImg(x, y) != 0) {
                whitePixels.insert(whitePixels.end(), PixelPosT(x, y));
            }
        }

    // Iterate over white pixels as long as set is not empty
    while (!whitePixels.empty()) {
        PixelPosListT pixelPosList;
        PixelPosListT pixelsToBeProcessed;

        auto itWhitePixPos = whitePixels.begin();

        pixelsToBeProcessed.push_back(*itWhitePixPos);

        while (!pixelsToBeProcessed.empty()) {
            PixelPosT curPixelPos = pixelsToBeProcessed.front();

            getAndRemoveNeighbours(curPixelPos, &whitePixels, &pixelsToBeProcessed, & pixelPosList);
            pixelsToBeProcessed.pop_front();
        }

        // Finally, append the cluster
        if (! pixelPosList.empty()) {
            recognizedClusters.push_back(PixelClusterT(pixelPosList));
        }
    }

    return recognizedClusters;
}
