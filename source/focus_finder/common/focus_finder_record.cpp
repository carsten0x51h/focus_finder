#include "include/focus_finder_record.h"
#include "include/fwhm.h"
#include "include/hfd.h"
#include "include/image.h"
#include "include/point.h"

FocusFinderRecordT::FocusFinderRecordT(int currentAbsoluteFocusPos, float snr,
		const FwhmT & fwhmHorz, const FwhmT & fwhmVert, const HfdT & hfd,
		const ImageT & roiImage, const ImageT & correctedStarImage,
		const PointFT & absStarCenterPos, const std::tuple<float,float> & drift) :
		mCurrentAbsoluteFocusPos(currentAbsoluteFocusPos), mSnr(snr), mFwhmHorz(
				fwhmHorz), mFwhmVert(fwhmVert), mHfd(hfd), mRoiImage(roiImage), mCorrectedStarImage(
				correctedStarImage), mAbsStarCenterPos(absStarCenterPos), mDrift(drift) {
}

int FocusFinderRecordT::getCurrentAbsoluteFocusPos() const {
	return mCurrentAbsoluteFocusPos;
}
float FocusFinderRecordT::getSnr() const {
	return mSnr;
}
const FwhmT & FocusFinderRecordT::getFwhmHorz() const {
	return mFwhmHorz;
}
const FwhmT & FocusFinderRecordT::getFwhmVert() const {
	return mFwhmVert;
}
const HfdT & FocusFinderRecordT::getHfd() const {
	return mHfd;
}
const ImageT & FocusFinderRecordT::getRoiImage() const {
	return mRoiImage;
}
const ImageT & FocusFinderRecordT::getCorrectedStarImage() const {
	return mCorrectedStarImage;
}
const PointFT & FocusFinderRecordT::getAbsStarCenterPos() const {
	return mAbsStarCenterPos;
}
const std::tuple<float, float> & FocusFinderRecordT::getDrift() const {
	return mDrift;
}

std::ostream &
FocusFinderRecordT::print(std::ostream & os) const {
	os << "Abs focus pos: " << mCurrentAbsoluteFocusPos
			<< ", SNR: " << mSnr
			<< ", Drift: " << mDrift << std::endl
			<< "Fwhm(HORZ): " << mFwhmHorz << std::endl
			<< "Fwhm(VERT): " << mFwhmVert << std::endl
			<< "Hfd: " << mHfd << std::endl
			<< "Star center: " << mAbsStarCenterPos << std::endl;
	return os;
}

std::ostream & operator<<(std::ostream & os,
		const FocusFinderRecordT & record) {
	return record.print(os);
}
