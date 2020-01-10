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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_BUILDER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_BUILDER_H_

#include <memory>
#include <tuple>
#include <chrono>

#include "focus_curve_record.h"
#include "fwhm.h"
#include "hfd.h"
#include "image.h"
#include "point.h"
#include "timestamp.h"

class FocusCurveRecordBuilderT {
private:
  TimestampT mCreationTimestamp;
  int mCurrentFocusPos;
  std::tuple<float, float> mDrift;
  PointFT mAbsStarCenterPos;
  std::chrono::duration<float> mExposureTime;

  float mSnr;
  FwhmT mFwhmHorz;
  FwhmT mFwhmVert;
  HfdT mHfd;
  ImageT mRoiImage;
  ImageT mCorrectedStarImage;
  
public:
	FocusCurveRecordBuilderT() :
			mCurrentFocusPos(0), mSnr(0), mAbsStarCenterPos(0, 0), mDrift(0, 0) {
		// TODO: Automatically sets the creation date and time (?)
	}

	FocusCurveRecordBuilderT & setCreationTimestamp(TimestampT creationTimestamp) {
		mCreationTimestamp = creationTimestamp;
		return (*this);
	}

  
	FocusCurveRecordBuilderT & setAbsoluteFocusPos(int currentFocusPos) {
		mCurrentFocusPos = currentFocusPos;
		return (*this);
	}

//	.setCenterCoords()						// In abs coords for UI to set cross / ROI frame...
//	.setDrift()								// dx, dy - how far did star center move since last shot

// SNR of the star image
	FocusCurveRecordBuilderT & setSnr(float snr) {
		mSnr = snr;
		return (*this);
	}

	// Horizontal Star FWHM
	FocusCurveRecordBuilderT & setHorzFwhm(const FwhmT & fwhmHorz) {
		mFwhmHorz = fwhmHorz;
		return (*this);
	}

	// Vertical Star FWHM
	FocusCurveRecordBuilderT & setVertFwhm(const FwhmT & fwhmVert) {
		mFwhmVert = fwhmVert;
		return (*this);
	}

	// Star HFD
	FocusCurveRecordBuilderT & setHfd(const HfdT & hfd) {
		mHfd = hfd;
		return (*this);
	}

	FocusCurveRecordBuilderT & setRoiImage(const ImageT & roiImage) {
		mRoiImage = roiImage;
		return (*this);
	}

	FocusCurveRecordBuilderT & setCorrectedStarImage(
			const ImageT & correctedStarImage) {
		mCorrectedStarImage = correctedStarImage;
		return (*this);
	}

	// In abs coords for UI to set cross / ROI frame...
	FocusCurveRecordBuilderT & setAbsStarCenterPos(
			const PointFT & absStarCenterPos) {
		mAbsStarCenterPos = absStarCenterPos;
		return (*this);
	}

	// dx, dy - how far did star center move since last shot
	FocusCurveRecordBuilderT & setDrift(
					const std::tuple<float, float> & drift) {
				mDrift = drift;
				return (*this);
			}

	FocusCurveRecordBuilderT & setExposureTime(
			std::chrono::duration<float> exposureTime) {
		mExposureTime = exposureTime;
		return (*this);
	}

  
	std::shared_ptr<FocusCurveRecordT> build() const {
	  return std::make_shared<FocusCurveRecordT>(mCreationTimestamp, mCurrentFocusPos, mExposureTime, mSnr,
				mFwhmHorz, mFwhmVert, mHfd, mRoiImage, mCorrectedStarImage,
				mAbsStarCenterPos, mDrift);
	}

};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_BUILDER_H_*/
