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

#include "include/draw_helper.h"

/**
 * Little draw helper function to draw a cross...
 */
void DrawHelperT::drawCross(cimg_library::CImg<unsigned char> *inoutImg,
                            float inX, float inY, const unsigned char *inColor, size_t inCrossSize,
                            float inScaleFactor, size_t inOpacity) {
    inoutImg->draw_line(floor(inScaleFactor * (inX - (float) inCrossSize + 1.0F) + 0.5),
                        floor(inScaleFactor * (inY + 1) + 0.5),
                        floor(inScaleFactor * (inX + (float) inCrossSize + 1.0F) + 0.5),
                        floor(inScaleFactor * (inY + 1) + 0.5), inColor, (float) inOpacity);

    inoutImg->draw_line(floor(inScaleFactor * (inX + 1) + 0.5),
                        floor(inScaleFactor * (inY - (float) inCrossSize + 1.0F) + 0.5),
                        floor(inScaleFactor * (inX + 1) + 0.5),
                        floor(inScaleFactor * (inY + (float) inCrossSize + 1.0F) + 0.5), inColor,
                        (float) inOpacity);
}

void DrawHelperT::drawCross(cimg_library::CImg<unsigned char> *inoutImg,
                            const PointT<float> &inPos, const unsigned char *inColor,
                            size_t inCrossSize, float inScaleFactor, size_t inOpacity) {
    drawCross(inoutImg, inPos.x(), inPos.y(), inColor,
              inCrossSize, inScaleFactor, inOpacity);
}
