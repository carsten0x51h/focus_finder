#include "include/star_cluster_algorithm.h"

void
StarClusterAlgorithmT::getAndRemoveNeighbours(PixelPosT inCurPixelPos, PixelPosSetT * inoutWhitePixels, 
					      StarClusterT * inoutPixelsToBeProcessed,
					      StarClusterT * outPixelCluster)
{
  const size_t _numPixels = 8, _x = 0, _y = 1;
  const int offsets[_numPixels][2] = { { -1, -1 }, { 0, -1 }, { 1, -1 },
				       { -1, 0 },              { 1, 0 },
				       { -1, 1 }, { 0, 1 }, { 1, 1 } };

  for (size_t p = 0; p < _numPixels; ++p) {
    PixelPosT curPixPos(std::get<0>(inCurPixelPos) + offsets[p][_x], std::get<1>(inCurPixelPos) + offsets[p][_y]);
    PixelPosSetT::iterator itPixPos = inoutWhitePixels->find(curPixPos);

    if (itPixPos != inoutWhitePixels->end()) {
      const PixelPosT & curPixPos = *itPixPos;
      inoutPixelsToBeProcessed->push_back(curPixPos);
      outPixelCluster->push_back(curPixPos);
      inoutWhitePixels->erase(itPixPos); // Remove white pixel from "white set" since it has been now processed
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
