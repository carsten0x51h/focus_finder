/*
 * star_info.cpp
 *
 *  Created on: Apr 27, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_FRAME_CPP_
#define SOURCE_FOCUS_FINDER_COMMON_FRAME_CPP_

#include "include/rect.h"
#include "include/frame.h"
#include "include/binning.h"


bool FrameT::isRoiSet() const {
	return mRoi.isSet();
}

FrameT::FrameT() :
	mRoi(0, 0, 0, 0),
	mBinning(1, 1)
{

}

void FrameT::setImage(std::shared_ptr<const ImageT> image) {
	mImage = image;
}
// TODO: Needs clearImage() ?

std::shared_ptr<const ImageT> FrameT::getImage() const {
	return mImage;
}

bool FrameT::isEmpty() const {
	return (mImage == nullptr);
}

// TODO: binning ...?!
void FrameT::setRoi(const RectT<unsigned int> & roi) {
	mRoi = roi;
}

// TODO: binning ...?!
const RectT<unsigned int> & FrameT::getRoi() const {
	return mRoi;
}

RectT<unsigned int> FrameT::getBounds() const {

	RectT<unsigned int> retRect; // by default "not set"

	if (mRoi.isSet()) {
		retRect = mRoi;
	}
	else if (mImage != nullptr) {
		retRect = RectT<unsigned int> (0,0,mImage->width(), mImage->height());
	}

	return retRect;
}

void FrameT::clearRoi() {
	mRoi.clear();
}

bool FrameT::isSubFrame() const {
	return ! isEmpty() && isRoiSet();
}

bool FrameT::isFullFrame() const {
	return ! isEmpty() && ! isRoiSet();
}

const BinningT & FrameT::getBinning() const {
	return mBinning;
}

void FrameT::setBinning(const BinningT & binning) {
	mBinning = binning;
}

#endif /* SOURCE_FOCUS_FINDER_COMMON_FRAME_CPP_ */
