/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */
#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_HYPERBOLIC_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_HYPERBOLIC_H_

#include <vector>
#include <array>
#include <atomic>
#include <chrono>

#include <thread>
#include <condition_variable>

#include <boost/signals2.hpp>

//#include <gsl/gsl_multifit_nlin.h>

#include "focus_finder.h"
//#include "curve_fit_tmpl.h"
#include "curve_fit_algorithm.h"
#//include "hyperbol_fit_traits.h"
#include "focus.h"


///////////////////////////////////////////////////////////////////////
// Custom data structure + accessor
///////////////////////////////////////////////////////////////////////
// TODO: Rename... My....
typedef std::pair<float, float> MyDataPointT;
typedef std::vector<MyDataPointT> MyDataContainerT;

class MyDataAccessorT {
public:
	typedef MyDataContainerT TypeT;
	static PointFT getDataPoint(size_t /*inIdx*/, TypeT::const_iterator inIt) {
		return PointFT(inIt->first /*inIdx*/, inIt->second /*y*/);
	}
};

class FocusFinderHyperbolicT: public FocusFinderT {
private:
	// Prevent copy
	FocusFinderHyperbolicT(const FocusFinderHyperbolicT &);
	FocusFinderHyperbolicT & operator=(const FocusFinderHyperbolicT &);

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

	std::condition_variable cv;
	std::mutex cvMutex;


public:
	FocusFinderHyperbolicT();

	// Implement focus finder interface
	std::string getName() const override;

	void run() override;
	void cancel() override;
	void reset() override;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_HYPERBOLIC_H_ */
