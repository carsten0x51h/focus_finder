/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */
#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_FAST_CURVE_LOOKUP_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_FAST_CURVE_LOOKUP_H_

#include <vector>
//#include <array>
#include <chrono>

//#include <atomic>
//#include <thread>

#include <boost/signals2.hpp>

#include "focus_finder.h"
#include "curve_fit_algorithm.h"
#include "focus_controller.h"
#include "boundary_location.h"
#include "focus.h"

class CurveFunctionT;

///////////////////////////////////////////////////////////////////////
// Custom data structure + accessor
///////////////////////////////////////////////////////////////////////
// TODO: Rename... My....
// TODO: Is this actually needed any more? + my DataAccessorT?
typedef std::pair<float, float> MyDataPointT;
typedef std::vector<MyDataPointT> MyDataContainerT;

class MyDataAccessorT {
public:
	typedef MyDataContainerT TypeT;
	static PointFT getDataPoint(size_t /*inIdx*/, TypeT::const_iterator inIt) {
		return PointFT(inIt->first /*inIdx*/, inIt->second /*y*/);
	}
};

class FocusFinderFastCurveLookupT: public FocusFinderT {
private:
  // Prevent copy
  FocusFinderFastCurveLookupT(const FocusFinderFastCurveLookupT &);
  FocusFinderFastCurveLookupT & operator=(const FocusFinderFastCurveLookupT &);
  
  //float estimateRelPos(std::shared_ptr<CurveFunctionT> focusCurveFunction, float focusMeasure, CurveHalfT::TypeE curveHalf);

  void rollbackFocus();
  void focusFinderCleanup();
  
  void checkCancelled() const;
  
  std::shared_ptr<const ImageT> mCurrentImage;
  
  boost::signals2::connection mCameraExposureFinishedConnection;
  
  std::atomic<bool> mCancelled;
  std::atomic<bool> mIsRunning;  
  int mInitialAbsPosition;
  
public:
  FocusFinderFastCurveLookupT(std::shared_ptr<FocusControllerT> focusController);

  // Implement focus finder interface
  std::string getName() const override;
  
  bool isRunning() const override;
  void run() override;
  void cancel() override;
  void reset() override;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_FAST_CURVE_LOOKUP_H_ */
