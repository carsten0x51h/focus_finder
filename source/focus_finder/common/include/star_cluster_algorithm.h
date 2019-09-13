#include <list>
#include <set>
#include <algorithm>

#include "point.h"
#include "image.h"

// TODO: Should this be here?
typedef PointT<int> PixelPosT;
typedef std::set<PixelPosT> PixelPosSetT;
typedef std::list<PixelPosT> StarClusterT;

/**
 * Usage:
 *
 * CImg<float> binImg;
 * std::list<StarClusterT> recognizedPixelClusters = StarClusterAlgorithmT::cluster(binImg);
 */
class StarClusterAlgorithmT {
private:
  /**
   * Removes all white neighbours arond pixel from whitePixels
   * if they exist and adds them to pixelsToBeProcessed and to
   * pixelsinCluster.
   */
  static void
  getAndRemoveNeighbours(PixelPosT inCurPixelPos, PixelPosSetT * inoutWhitePixels, 
			 StarClusterT * inoutPixelsToBeProcessed,
			 StarClusterT * outPixelCluster);
  
public:
  static std::list<StarClusterT> cluster(const ImageT & inImg);
};
