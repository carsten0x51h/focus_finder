#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEFAULT_FOCUS_CURVE_RECORDER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEFAULT_FOCUS_CURVE_RECORDER_H_

#include <atomic>

#include <thread>
#include <condition_variable>

#include <boost/signals2.hpp>

#include "focus_curve_recorder.h"


class DefaultFocusCurveRecorderT: public FocusCurveRecorderT {
private:
	// Prevent copy
	DefaultFocusCurveRecorderT(const DefaultFocusCurveRecorderT &);
	DefaultFocusCurveRecorderT & operator=(const DefaultFocusCurveRecorderT &);

    /* 	void onImageReceived(RectT<unsigned int> roi, */
    /* 			std::shared_ptr<const ImageT> image, bool lastFrame); */

    /* 	void focusFinderCleanup(); */

    /* 	void checkIfStarIsThere(const ImageT & img, float * outSnr = 0) const; */
    /* 	void runExposureBlocking(std::chrono::milliseconds expTime); */
    /* 	void moveFocusByBlocking(FocusDirectionT::TypeE direction, int ticks, std::chrono::milliseconds timeout); */
    /* 	void checkCancelled() const; */

    /* 	std::shared_ptr<const ImageT> mCurrentImage; */

    /* 	boost::signals2::connection mCameraExposureFinishedConnection; */

  std::atomic<bool> mCancelled;
  std::atomic<bool> mIsRunning;

  std::condition_variable cv;
  std::mutex cvMutex;


public:
	DefaultFocusCurveRecorderT();

	// Implement focus curve recorder interface
	std::string getName() const override;

    bool isRunning() const override;
    void run() override;
	void cancel() override;
	void reset() override;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEFAULT_FOCUS_CURVE_RECORDER_H_*/
