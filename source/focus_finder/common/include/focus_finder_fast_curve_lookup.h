/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */
#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_FAST_CURVE_LOOKUP_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_FAST_CURVE_LOOKUP_H_

#include <vector>
#include <array>
#include <atomic>
#include <chrono>

#include <thread>
#include <condition_variable>

#include <boost/signals2.hpp>

#include "focus_finder.h"
#include "curve_fit_algorithm.h"
#include "focus_controller.h"
#include "focus.h"


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

	void onImageReceived(RectT<unsigned int> roi,
			std::shared_ptr<const ImageT> image, bool lastFrame);

	void focusFinderCleanup();

	void checkIfStarIsThere(const ImageT & img, float * outSnr = 0) const;
	void runExposureBlocking(std::chrono::milliseconds expTime);
	void moveFocusByBlocking(FocusDirectionT::TypeE direction, int ticks, std::chrono::milliseconds timeout);
	void checkCancelled() const;

	std::shared_ptr<const ImageT> mCurrentImage;

	boost::signals2::connection mCameraExposureFinishedConnection;

	std::atomic<bool> mCancelled;
    std::atomic<bool> mIsRunning;

	std::condition_variable cv;
	std::mutex cvMutex;


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
