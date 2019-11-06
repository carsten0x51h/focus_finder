#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_H_

#include <ctime>

#include "fwhm.h"
#include "hfd.h"
#include "image.h"
#include "point.h"
#include "focus_measure_type.h"
#include "exception.h"

DEF_Exception(FocusCurveRecord);

class FocusCurveRecordT : public std::enable_shared_from_this<FocusCurveRecordT> {
private:
	int mCurrentAbsoluteFocusPos;
	float mSnr;
	FwhmT mFwhmHorz;
	FwhmT mFwhmVert;
	HfdT mHfd;
	ImageT mRoiImage;
	ImageT mCorrectedStarImage;
	PointFT mAbsStarCenterPos;
	std::tuple<float, float> mDrift;
  std::time_t mDateTime;

public:
	FocusCurveRecordT(int currentAbsoluteFocusPos, float snr,
			const FwhmT & fwhmHorz, const FwhmT & fwhmVert, const HfdT & hfd,
			const ImageT & roiImage, const ImageT & correctedStarImage, const PointFT & absStarCenterPos, const std::tuple<float,float> & drift);

	int getCurrentAbsoluteFocusPos() const;
	float getSnr() const;
	const FwhmT & getFwhmHorz() const;
	const FwhmT & getFwhmVert() const;
	const HfdT & getHfd() const;
	const ImageT & getRoiImage() const;
	const ImageT & getCorrectedStarImage() const;
	const PointFT & getAbsStarCenterPos() const;
	const std::tuple<float, float> & getDrift() const;
  std::time_t getDateTime() const;

  static float getFocusMeasure(std::shared_ptr<FocusCurveRecordT> focusCurveRecord, FocusMeasureTypeT::TypeE focusMeasureType);
  float getFocusMeasure(FocusMeasureTypeT::TypeE focusMeasureType);
  
	std::ostream & print(std::ostream & os) const;
	friend std::ostream & operator<<(std::ostream & os, const FocusCurveRecordT & record);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_H_*/
