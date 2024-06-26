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

#include "include/rect.h"
#include "include/frame.h"

#include <utility>
#include "include/binning.h"


bool FrameT::isRoiSet() const {
    return mRoi.isSet();
}

FrameT::FrameT() :
        mRoi(0, 0, 0, 0),
        mBinning(1, 1) {

}

void FrameT::setImage(std::shared_ptr<const ImageT> image) {
    mImage = std::move(image);
}
// TODO: Needs clearImage() ?

std::shared_ptr<const ImageT> FrameT::getImage() const {
    return mImage;
}

bool FrameT::isEmpty() const {
    return (mImage == nullptr);
}

// TODO: binning ...?!
void FrameT::setRoi(const RectT<unsigned int> &roi) {
    mRoi = roi;
}

// TODO: binning ...?!
const RectT<unsigned int> &FrameT::getRoi() const {
    return mRoi;
}

RectT<unsigned int> FrameT::getBounds() const {

    RectT<unsigned int> retRect; // by default "not set"

    if (mRoi.isSet()) {
        retRect = mRoi;
    } else if (mImage != nullptr) {
        retRect = RectT<unsigned int>(0, 0, mImage->width(), mImage->height());
    }

    return retRect;
}

void FrameT::clearRoi() {
    mRoi.clear();
}

bool FrameT::isSubFrame() const {
    return !isEmpty() && isRoiSet();
}

bool FrameT::isFullFrame() const {
    return !isEmpty() && !isRoiSet();
}

const BinningT &FrameT::getBinning() const {
    return mBinning;
}

void FrameT::setBinning(const BinningT &binning) {
    mBinning = binning;
}
