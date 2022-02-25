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

#include "include/centroid_view.h"
#include "include/draw_helper.h"

const float CentroidViewT::defaultScaleFactor = 4.0;

cimg_library::CImg<unsigned char> CentroidViewT::genView(
        const CentroidT &centroid, float scaleFactor) {

    cimg_library::CImg<unsigned char> rgbImg;

    auto centerOpt = centroid.getCenter();

    if (centerOpt.has_value()) {
        PointT<float> center = centerOpt.value();

        const ImageT &centroidResultImage = centroid.getResultImage();

        rgbImg.assign(centroidResultImage.width(), centroidResultImage.height(),
                      1 /*depth*/, 3 /*3 channels - RGB*/);

        // TODO: We may use the normalize function instead...
        const float min = centroidResultImage.min();
        const float mm = centroidResultImage.max() - min;

        cimg_forXY(centroidResultImage, x, y) {
                int value = 255.0 * (centroidResultImage(x, y) - min) / mm;
                rgbImg(x, y, 0 /*red*/) = value;
                rgbImg(x, y, 1 /*green*/) = value;
                rgbImg(x, y, 2 /*blue*/) = value;
            }

        // Draw center cross
        const unsigned char red[3] = {255, 0, 0};

        // Scale image
        rgbImg.resize(scaleFactor * rgbImg.width(),
                      scaleFactor * rgbImg.height(), -100 /*size_z*/, -100 /*size_c*/,
                      1 /*interpolation_type*/);

        DrawHelperT::drawCross(&rgbImg,
                               floor(scaleFactor * std::get<0>(center) + 0.5),
                               floor(scaleFactor * std::get<1>(center) + 0.5), red,
                               3 /*cross-size*/, 1.0 /*scale factor*/, 1 /*opacity*/);
    }
    return rgbImg; // Make a copy...
}
