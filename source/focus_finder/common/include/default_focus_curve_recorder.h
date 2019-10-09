#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEFAULT_FOCUS_CURVE_RECORDER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_DEFAULT_FOCUS_CURVE_RECORDER_H_

#include <atomic>

#include <thread>
#include <condition_variable>

#include <boost/signals2.hpp>

#include "focus_curve_recorder.h"


// TODO / IDEA: Maybe HfdT and FwhmT should both implement a generic inteface "FocusMeasureT" which just hast "float getValue()".


class FocusCurveRecordT;


// TODO: enum struct in own header file? Better name?
enum CurveHalfE { LeftHalf, RightHalf };

// TODO: Move to sep. class...?
struct SelfOrientationResultT {
  FocusDirectionT::TypeE focusDirectionToLimit;
  CurveHalfE curveHalf;
  std::shared_ptr<FocusCurveRecordT> record1;
  std::shared_ptr<FocusCurveRecordT> record2;
};


class DefaultFocusCurveRecorderT: public FocusCurveRecorderT {
private:
	// Prevent copy
	DefaultFocusCurveRecorderT(const DefaultFocusCurveRecorderT &);
	DefaultFocusCurveRecorderT & operator=(const DefaultFocusCurveRecorderT &);


  void devicesAvailabilityCheck();
  void cleanup();
  void checkCancelled() const;
  void waitForFocus(std::chrono::milliseconds timeout) const;
  void moveFocusToBlocking(int absPos, std::chrono::milliseconds timeout);
  void moveFocusByBlocking(FocusDirectionT::TypeE direction, int ticks, std::chrono::milliseconds timeout);
  SelfOrientationResultT performSelfOrientation();
  void moveUntilFocusMeasureLimitReached(const SelfOrientationResultT & selfOrientationResult, float stepSize, float focusMeasureLimit);
  CurveHalfE locateStartingPosition();
  std::shared_ptr<FocusCurveRecordT> measureFocus();
  void recordFocusCurve(CurveHalfE curveHalf);

  
  void onImageReceived(RectT<unsigned int> roi,
		       std::shared_ptr<const ImageT> image, bool lastFrame);


  void checkIfStarIsThere(const ImageT & img, float * outSnr = 0) const;
  void runExposureBlocking(std::chrono::milliseconds expTime);

  std::shared_ptr<const ImageT> mCurrentImage;

  boost::signals2::connection mCameraExposureFinishedConnection;

  std::atomic<bool> mCancelled;
  std::atomic<bool> mIsRunning;

  std::condition_variable cv;
  std::mutex cvMutex;

  int mInitialFocusPos;
  int mStepSize;
  float mFocusMeasureLimit;
  
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
