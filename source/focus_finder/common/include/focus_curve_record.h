#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_H_

#include "fwhm.h"
#include "hfd.h"
#include "image.h"
#include "point.h"
#include "focus_measure_type.h"
#include "exception.h"
#include "timestamp.h"

#include <filesystem>

// TODO: Remove, when store() function has been extracted...
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

DEF_Exception(FocusCurveRecord);

class FocusCurveRecordT : public std::enable_shared_from_this<FocusCurveRecordT> {
private:
  TimestampT mCreationTimestamp;
  int mCurrentAbsoluteFocusPos;
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
  FocusCurveRecordT(TimestampT creationTimestamp, int currentAbsoluteFocusPos, std::chrono::duration<float> exposureTime, float snr,
			const FwhmT & fwhmHorz, const FwhmT & fwhmVert, const HfdT & hfd,
			const ImageT & roiImage, const ImageT & correctedStarImage, const PointFT & absStarCenterPos, const std::tuple<float,float> & drift);

  TimestampT getCreationTimestamp() const;
  int getCurrentAbsoluteFocusPos() const;
  std::chrono::duration<float> getExposureTime() const;

  // TODO: Do not store those as members but only calc if required ... based on RoiImage(?) or correctedStarImage...?
  // -> Even store "focus measure" in curve record?? 
  float getSnr() const;
	const FwhmT & getFwhmHorz() const;
	const FwhmT & getFwhmVert() const;
	const HfdT & getHfd() const;
  
	const ImageT & getRoiImage() const;
	const ImageT & getCorrectedStarImage() const;
	const PointFT & getAbsStarCenterPos() const;
	const std::tuple<float, float> & getDrift() const;

  static float getFocusMeasure(std::shared_ptr<FocusCurveRecordT> focusCurveRecord, FocusMeasureTypeT::TypeE focusMeasureType);
  float getFocusMeasure(FocusMeasureTypeT::TypeE focusMeasureType);
  
  std::ostream & print(std::ostream & os, size_t indent = 0) const;
  friend std::ostream & operator<<(std::ostream & os, const FocusCurveRecordT & record);

  
  // TODO: Those functions may be moved out of this class because the dependency to property_tree shoud not be in here... It does not have to be a class member at all!
  static std::shared_ptr<FocusCurveRecordT> load(const boost::property_tree::ptree & pt);
  static void save(/*const std::filesystem::path & lightFrameDirectoryPath,*/ boost::property_tree::ptree & pt, const FocusCurveRecordT & focusCurveRecord);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_RECORD_H_*/
