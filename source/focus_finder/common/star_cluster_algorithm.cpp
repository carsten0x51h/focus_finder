#include "include/star_cluster_algorithm.h"
#include "include/logging.h"


StarClusterAlgorithmT::StarClusterAlgorithmT(size_t clusterRadius) {
  initOffsetPattern((int)clusterRadius);
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
StarClusterAlgorithmT::getAndRemoveNeighbours(const PixelPosT & inCurPixelPos, PixelPosSetT * inoutWhitePixels, 
					      StarClusterT * inoutPixelsToBeProcessed,
					      StarClusterT * outPixelCluster)
{
  // TODO: Does not work!
  
  for (const PixelPosT & offset : mOffsets) {
    PixelPosT curPixPos(inCurPixelPos.x() + offset.x(), inCurPixelPos.y() + offset.y());
    
    PixelPosSetT::iterator itPixPos = inoutWhitePixels->find(curPixPos);

    if (itPixPos != inoutWhitePixels->end()) {
      const PixelPosT & curWhitePixPos = *itPixPos;
      inoutPixelsToBeProcessed->push_back(curWhitePixPos);
      outPixelCluster->push_back(curWhitePixPos);
      inoutWhitePixels->erase(itPixPos); // Remove white pixel from "white set" since it has been processed, now
    }
  }
}


std::list<StarClusterT> StarClusterAlgorithmT::cluster(const ImageT & inImg) {
  std::list<StarClusterT> recognizedClusters;
  PixelPosSetT whitePixels;

  cimg_forXY(inImg, x, y) {
    if (inImg(x, y) != 0) {
      whitePixels.insert(whitePixels.end(), PixelPosT(x, y));
    }
  }

  // Iterate over white pixels as long as set is not empty
  while (! whitePixels.empty()) {
    StarClusterT pixelCluster;
    StarClusterT pixelsToBeProcessed;

    PixelPosSetT::iterator itWhitePixPos = whitePixels.begin();

    pixelsToBeProcessed.push_back(*itWhitePixPos);
    whitePixels.erase(itWhitePixPos);

    while(! pixelsToBeProcessed.empty()) {
      PixelPosT curPixelPos = pixelsToBeProcessed.front();

      getAndRemoveNeighbours(curPixelPos, & whitePixels, & pixelsToBeProcessed, & pixelCluster);
      pixelsToBeProcessed.pop_front();
    }

    // Finally, append the cluster
    recognizedClusters.push_back(pixelCluster);
  }

  return recognizedClusters;
}
