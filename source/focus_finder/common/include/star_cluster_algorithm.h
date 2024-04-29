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

#include <list>
#include <vector>
#include <set>
#include <algorithm>

#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/minmax.hpp>

#include "point.h"
#include "rect.h"
#include "image.h"

// TODO: Should this be here?
typedef PointT<int> PixelPosT;
typedef std::list<PixelPosT> PixelPosListT;
typedef std::set<PixelPosT> PixelPosSetT;


/**
 *
 */
class PixelClusterT {
private:
	PixelPosListT mPixelPositions;

public:
	PixelClusterT(const PixelPosListT & pixelPositions) : mPixelPositions(pixelPositions) {}
	const PixelPosListT & getPixelPositions() const { return mPixelPositions; }
	RectT<int> getBounds() const;
};


/**
 * Usage:
 *
 * CImg<float> binaryImg;
 * StarClusterAlgorithmT starClusterAlgorithm(2);
 * std::list<PixelClusterT> clusters = starClusterAlgorithm.cluster(binaryImg);
 */
class StarClusterAlgorithmT {
private:
    std::vector<PixelPosT> mOffsets;

    /**
     *
     */
    void initOffsetPattern(int n);

    /**
     * Removes all white neighbours around pixel from whitePixels
     * if they exist and adds them to pixelsToBeProcessed and to
     * pixelsinCluster.
     */
    void
    getAndRemoveNeighbours(const PixelPosT &inCurPixelPos, PixelPosSetT *inoutWhitePixels,
    		PixelPosListT *inoutPixelsToBeProcessed,
			PixelPosListT *outPixelCluster);

public:
    explicit StarClusterAlgorithmT(size_t clusterRadius);

    std::list<PixelClusterT> cluster(const ImageT &inImg);
};
