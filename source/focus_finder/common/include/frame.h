/*
 * focus.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

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

	void setRoi(const RectT<unsigned int> & roi);
	const RectT<unsigned int> & getRoi() const;
	void clearRoi();

	RectT<unsigned int> getBounds() const;

	bool isSubFrame() const;
	bool isFullFrame() const;

	const BinningT & getBinning() const;
	void setBinning(const BinningT & binning);

private:
	bool isRoiSet() const;

	RectT<unsigned int> mRoi; // TODO: Is unsigned int always ok?
	std::shared_ptr<const ImageT> mImage;
	BinningT mBinning;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FRAME_H_ */


