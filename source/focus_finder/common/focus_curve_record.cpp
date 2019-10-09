#include "include/focus_curve_record.h"
#include "include/fwhm.h"
#include "include/hfd.h"
#include "include/image.h"
#include "include/point.h"

FocusCurveRecordT::FocusCurveRecordT(int currentAbsoluteFocusPos, float snr,
		const FwhmT & fwhmHorz, const FwhmT & fwhmVert, const HfdT & hfd,
		const ImageT & roiImage, const ImageT & correctedStarImage,
		const PointFT & absStarCenterPos, const std::tuple<float,float> & drift) :
		mCurrentAbsoluteFocusPos(currentAbsoluteFocusPos), mSnr(snr), mFwhmHorz(
				fwhmHorz), mFwhmVert(fwhmVert), mHfd(hfd), mRoiImage(roiImage), mCorrectedStarImage(
				correctedStarImage), mAbsStarCenterPos(absStarCenterPos), mDrift(drift) {
}

int FocusCurveRecordT::getCurrentAbsoluteFocusPos() const {
	return mCurrentAbsoluteFocusPos;
}
float FocusCurveRecordT::getSnr() const {
	return mSnr;
}
const FwhmT & FocusCurveRecordT::getFwhmHorz() const {
	return mFwhmHorz;
}
const FwhmT & FocusCurveRecordT::getFwhmVert() const {
	return mFwhmVert;
}
const HfdT & FocusCurveRecordT::getHfd() const {
	return mHfd;
}
const ImageT & FocusCurveRecordT::getRoiImage() const {
	return mRoiImage;
}
const ImageT & FocusCurveRecordT::getCorrectedStarImage() const {
	return mCorrectedStarImage;
}
const PointFT & FocusCurveRecordT::getAbsStarCenterPos() const {
	return mAbsStarCenterPos;
}
const std::tuple<float, float> & FocusCurveRecordT::getDrift() const {
	return mDrift;
}

std::ostream &
FocusCurveRecordT::print(std::ostream & os) const {
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
		const FocusCurveRecordT & record) {
	return record.print(os);
}
