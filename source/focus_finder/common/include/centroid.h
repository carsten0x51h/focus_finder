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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CENTROID_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CENTROID_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CENTROID_H_

#include <valarray>
#include <optional>

#include "image.h"
#include "exception.h"
#include "logging.h"
#include "throw_if.h"
#include "point.h"
#include "rect.h"
#include "centroid_type.h"


DEF_Exception(Centroid);

class CentroidT {
private:
    ImageT mImg;
    std::optional<PointT<float> > mCenterOpt;

    /**
     * The star centroid algorithm is based on the code from: http://aladin.u-strasbg.fr/java/Plugins/Centroid.java
     * Further information on image moments can be found on wikipedia: http://de.wikipedia.org/wiki/Moment_%28Bildverarbeitung%29
     *
     * Also see http://www.isprs.org/proceedings/XXXV/congress/comm3/papers/341.pdf
     */
    static void calcCentroid2ndMoment(const ImageT &inImg,
                                      PointT<float> *outCentroidPos);

    static void calcCentroidSubPixel(const ImageT &inImg,
                                     PointT<float> inCenter, PointT<float> *outSubPixelCenter,
                                     size_t inNumIterations = 10);

    static float calcIx2(const ImageT &img, int x);

    static float calcJy2(const ImageT &img, int y);

    /**
     * Calculate Intensity Weighted Center (IWC).
     */
    static void calcIntensityWeightedCenter(const ImageT &inImg,
                                            PointT<float> *outCentroidPos);

public:
    CentroidT();

    explicit CentroidT(const ImageT &inImg,
              CentroidTypeT::TypeE inCalcType = CentroidTypeT::IWC,
              bool inSubMean = true);

    void set(const ImageT &inImg,
             CentroidTypeT::TypeE inCalcType = CentroidTypeT::IWC,
             bool inSubMean = true);

    static std::optional<PointT<float> > calculate(const ImageT &inImg,
                                                   CentroidTypeT::TypeE inCalcType = CentroidTypeT::IWC,
                                                   bool inSubMean = true,
                                                   ImageT *outImg = nullptr);

    [[nodiscard]] bool valid() const;

    [[nodiscard]] const ImageT &getResultImage() const;

    [[nodiscard]] std::optional<PointT<float> > getCenter() const;
};

#endif // SOURCE_FOCUS_FINDER_COMMON_INCLUDE_CENTROID_H_
