#include <chrono>

// TODO: Remove, when store() function has been extracted...
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <filesystem>


#include "include/focus_curve_record.h"
#include "include/fwhm.h"
#include "include/hfd.h"
#include "include/image.h"
#include "include/point.h"
#include "include/timestamp.h"
#include "include/timestamp_ptree_translator.h"
#include "include/point_ptree_translator.h"
#include "include/duration_ptree_translator.h"

#include "include/focus_curve_record_builder.h"

FocusCurveRecordT::FocusCurveRecordT(TimestampT creationTimestamp, int currentAbsoluteFocusPos, std::chrono::duration<float> exposureTime, float snr,
		const FwhmT & fwhmHorz, const FwhmT & fwhmVert, const HfdT & hfd,
		const ImageT & roiImage, const ImageT & correctedStarImage,
		const PointFT & absStarCenterPos, const std::tuple<float,float> & drift) :
  mCreationTimestamp(creationTimestamp), mCurrentAbsoluteFocusPos(currentAbsoluteFocusPos), mExposureTime(exposureTime), mSnr(snr), mFwhmHorz(
				fwhmHorz), mFwhmVert(fwhmVert), mHfd(hfd), mRoiImage(roiImage), mCorrectedStarImage(
				correctedStarImage), mAbsStarCenterPos(absStarCenterPos), mDrift(drift) {
}

TimestampT FocusCurveRecordT::getCreationTimestamp() const  {
  return mCreationTimestamp;
}

int FocusCurveRecordT::getCurrentAbsoluteFocusPos() const {
	return mCurrentAbsoluteFocusPos;
}

std::chrono::duration<float> FocusCurveRecordT::getExposureTime() const {
	return mExposureTime;
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

float FocusCurveRecordT::getFocusMeasure(std::shared_ptr<FocusCurveRecordT> focusCurveRecord, FocusMeasureTypeT::TypeE focusMeasureType) {
  
  switch (focusMeasureType) {
  case FocusMeasureTypeT::HFD:    
    return focusCurveRecord->getHfd().getValue();
  case FocusMeasureTypeT::FWHM_VERT:
    return focusCurveRecord->getFwhmVert().getValue();
  case FocusMeasureTypeT::FWHM_HORZ:
    return focusCurveRecord->getFwhmHorz().getValue();
  case FocusMeasureTypeT::FWHM_AVERAGE:
    return (focusCurveRecord->getFwhmHorz().getValue() + focusCurveRecord->getFwhmVert().getValue()) / 2.0F;
  default:
    throw FocusCurveRecordExceptionT("Invalid focus measure type.");
  }
  return 0.0;
}

float FocusCurveRecordT::getFocusMeasure(FocusMeasureTypeT::TypeE focusMeasureType) {
  return getFocusMeasure(shared_from_this(), focusMeasureType);
}


std::ostream &
FocusCurveRecordT::print(std::ostream & os, size_t indent) const {
  std::string prefix = std::string(indent, ' ');
  
  os << prefix << "Creation timestamp: " << mCreationTimestamp << std::endl
     << prefix << "Abs focus pos: " << mCurrentAbsoluteFocusPos << std::endl
     << prefix << "Drift: " << mDrift << std::endl
     << prefix << "Abs star center: " << mAbsStarCenterPos << std::endl
     << prefix << "Exposure time: " << mExposureTime.count() << "s" << std::endl;
    
     // << prefix << "SNR: " << mSnr << std::endl
     // << prefix << "Fwhm(HORZ): " << mFwhmHorz << std::endl
     // << prefix << "Fwhm(VERT): " << mFwhmVert << std::endl
     // << prefix << "Hfd: " << mHfd << std::endl;
  
  return os;
}

// TODO: This function may be moved out of this class because the dependency to property_tree shoud not be in here... It does not have to be a class member at all! -> Move to a sep. "translator" or "serialization" class... Maybe this should go to a translator function like for the enum? No, because probably this is only for one entry!
void FocusCurveRecordT::store(const std::filesystem::path & lightFrameDirectoryPath, const FocusCurveRecordT & focusCurveRecord, boost::property_tree::ptree & pt) {

  // // Create directory if it does not exist
  // // TODO: Also the ProfileManager should care about the creation of all required directories... (logic below...)
  // // TODO: Pass std::error_code& ec to handle error instead of throw...
  // std::filesystem::create_directories(lightFrameDirectoryPath);

  // // Create frame filename
  // long msSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(focusCurveRecord.getCreationTimestamp().time_since_epoch()).count();

  // std::stringstream ssImgFilename;
  // ssImgFilename << msSinceEpoch << ".tiff";

  
  // // Finally, build image file path
  // std::filesystem::path imgFilePath(lightFrameDirectoryPath);
  // imgFilePath /= ssImgFilename.str();

  // LOG(debug) << "Storing FocusCurveRecordT image to '" << imgFilePath << "'..." << std::endl;

  // // Save the image
  // // TODO: Error handling (permissions etc...)
  // const ImageT & img = focusCurveRecord.getCorrectedStarImage();
  // img.save_tiff(imgFilePath.string().c_str());

  
  // // Then add the properties to the property tree
  // pt.put<TimestampT>("curve_record.<xmlattr>.record_timestamp", msSinceEpoch);
  // pt.put<int>("curve_record.<xmlattr>.abs_focus_pos", focusCurveRecord.getCurrentAbsoluteFocusPos());
  // // TODO: Store later... -> instead of tuple use pair or create own type for which a translator is/will be defined...
  // //pt.put<std::tuple<float, float> >("curve_record.<xml_attr>.star_drift", focusCurveRecord.getDrift());

  
  // // TODO: Why the absolute star center and not the relative one(relative to saved star image?)
  // // TODO: Is this even required / should this be stored?
  // //pt.put<PointT<float> >("focus_curve_record.abs_star_center", focusCurveRecord.getAbsStarCenterPos());
}



std::shared_ptr<FocusCurveRecordT> FocusCurveRecordT::load(const boost::property_tree::ptree & pt /*, const std::filesystem::path & lightFrameDirectoryPath*/) {

    auto record = FocusCurveRecordBuilderT()
      .setCreationTimestamp(pt.get<TimestampT>("<xmlattr>.record_timestamp"))
      .setAbsoluteFocusPos(pt.get<int>("<xmlattr>.abs_focus_pos"))
      .setDrift(pt.get<PointFT>("<xmlattr>.star_drift"))
      .setAbsStarCenterPos(pt.get<PointFT>("<xmlattr>.abs_star_center_pos"))
      .setExposureTime(pt.get<std::chrono::duration<float> >("<xmlattr>.exposure_time"))
      // .setSnr(snr)
      // .setHorzFwhm(fwhmHorz)
      // .setVertFwhm(fwhmVert)
      // .setHfd(hfd)
      // .setRoiImage(averageCurrentImage) // Take a copy
      // .setCorrectedStarImage(innerCorrectedSubFrameImg)
      // .setAbsStarCenterPos(newCentroidAbsRoiCoords)
      .build();

    // DEBUG START
    LOG(debug) << "FocusCurveRecordT::load... Loaded FocusCurveRecordT: " << std::endl
	       << *record
	       << std::endl;
    // DEBUG END
    
    return record;
  // Create directory if it does not exist
  // TODO: Also the ProfileManager should care about the creation of all required directories... (logic below...)
  // TODO: Pass std::error_code& ec to handle error instead of throw...
  //std::filesystem::create_directories(lightFrameDirectoryPath);


  
  // Create frame filename
  //long msSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(focusCurveRecord.getDateTime().time_since_epoch()).count();

  // std::stringstream ssImgFilename;
  // ssImgFilename << msSinceEpoch << ".tiff";

  
  // Finally, build image file path
  // std::filesystem::path imgFilePath(lightFrameDirectoryPath);
  // imgFilePath /= ssImgFilename.str();

  // LOG(debug) << "Storing FocusCurveRecordT image to '" << imgFilePath << "'..." << std::endl;

  // // Save the image
  // // TODO: Error handling (permissions etc...)
  // ImageT img;eRecord.getCorrectedStarImage();
  // img.load_tiff(imgFilePath.string().c_str());

  
  // Then add the properties to the property tree
  // TODO: Store later... -> instead of tuple use pair or create own type for which a translator is/will be defined...
  //pt.put<std::tuple<float, float> >("curve_record.<xml_attr>.star_drift", focusCurveRecord.getDrift());

  
  // TODO: Why the absolute star center and not the relative one(relative to saved star image?)
  // TODO: Is this even required / should this be stored?
  //pt.put<PointT<float> >("focus_curve_record.abs_star_center", focusCurveRecord.getAbsStarCenterPos());
}




// NOTE: For reading of time_point / duration, use something like
// std::chrono::duration<long, std::milliseconds> ts(....value from file...);
// See https://en.cppreference.com/w/cpp/chrono/duration/duration
// Then create a time_pint from duration: explicit time_point (const duration& dtn);
// See http://www.cplusplus.com/reference/chrono/time_point/time_point/



std::ostream & operator<<(std::ostream & os,
		const FocusCurveRecordT & record) {
	return record.print(os);
}
