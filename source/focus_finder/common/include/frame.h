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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FRAME_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FRAME_H_

#include <memory>

#include "image.h"
#include "rect.h"
#include "binning.h"

class FrameT {
public:
    FrameT();

    void setImage(std::shared_ptr<const ImageT> image);

    // TODO: Does this return only the ROI as image or the entire image???
    std::shared_ptr<const ImageT> getImage() const;

    bool isEmpty() const; // true if no image is set

    void setRoi(const RectT<unsigned int> &roi);

    const RectT<unsigned int> &getRoi() const;

    void clearRoi();

    RectT<unsigned int> getBounds() const;

    bool isSubFrame() const;

    bool isFullFrame() const;

    const BinningT &getBinning() const;

    void setBinning(const BinningT &binning);

private:
    bool isRoiSet() const;

    RectT<unsigned int> mRoi; // TODO: Is unsigned int always ok?
    std::shared_ptr<const ImageT> mImage;
    BinningT mBinning;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FRAME_H_ */


