#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_BUILDER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_BUILDER_H_

#include <memory>
#include <tuple>

#include "focus_finder_record.h"
#include "fwhm.h"
#include "hfd.h"
#include "image.h"
#include "point.h"

class FocusFinderRecordBuilderT {
private:
	int mCurrentFocusPos;
	int mSnr;
	FwhmT mFwhmHorz;
	FwhmT mFwhmVert;
	HfdT mHfd;
	ImageT mRoiImage;
	ImageT mCorrectedStarImage;
	PointFT mAbsStarCenterPos;
	std::tuple<float, float> mDrift;

public:
	FocusFinderRecordBuilderT() :
			mCurrentFocusPos(0), mSnr(0), mAbsStarCenterPos(0, 0), mDrift(0, 0) {
		// TODO: Automatically sets the creation date and time (?)
	}

	FocusFinderRecordBuilderT & setAbsoluteFocusPos(int currentFocusPos) {
		mCurrentFocusPos = currentFocusPos;
		return (*this);
	}

//	.setCenterCoords()						// In abs coords for UI to set cross / ROI frame...
//	.setDrift()								// dx, dy - how far did star center move since last shot

// SNR of the star image
	FocusFinderRecordBuilderT & setSnr(float snr) {
		mSnr = snr;
		return (*this);
	}

	// Horizontal Star FWHM
	FocusFinderRecordBuilderT & setHorzFwhm(const FwhmT & fwhmHorz) {
		mFwhmHorz = fwhmHorz;
		return (*this);
	}

	// Vertical Star FWHM
	FocusFinderRecordBuilderT & setVertFwhm(const FwhmT & fwhmVert) {
		mFwhmVert = fwhmVert;
		return (*this);
	}

	// Star HFD
	FocusFinderRecordBuilderT & setHfd(const HfdT & hfd) {
		mHfd = hfd;
		return (*this);
	}

	FocusFinderRecordBuilderT & setRoiImage(const ImageT & roiImage) {
		mRoiImage = roiImage;
		return (*this);
	}

	FocusFinderRecordBuilderT & setCorrectedStarImage(
			const ImageT & correctedStarImage) {
		mCorrectedStarImage = correctedStarImage;
		return (*this);
	}

	// In abs coords for UI to set cross / ROI frame...
	FocusFinderRecordBuilderT & setAbsStarCenterPos(
			const PointFT & absStarCenterPos) {
		mAbsStarCenterPos = absStarCenterPos;
		return (*this);
	}

	// dx, dy - how far did star center move since last shot
	FocusFinderRecordBuilderT & setDrift(
					const std::tuple<float, float> & drift) {
				mDrift = drift;
				return (*this);
			}

	std::shared_ptr<FocusFinderRecordT> build() const {
		return std::make_shared<FocusFinderRecordT>(mCurrentFocusPos, mSnr,
				mFwhmHorz, mFwhmVert, mHfd, mRoiImage, mCorrectedStarImage,
				mAbsStarCenterPos, mDrift);
	}

};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_BUILDER_H_*/
