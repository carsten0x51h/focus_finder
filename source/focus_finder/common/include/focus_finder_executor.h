/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_EXECUTOR_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_EXECUTOR_H_

#include <memory>

#include "logging.h"

class FocusFinderT;

class FocusFinderExecutorT {
private:
	// Prevent copy of FocusFinder
	FocusFinderExecutorT(const FocusFinderT &);
	FocusFinderExecutorT & operator=(const FocusFinderExecutorT &);

public:
	FocusFinderExecutorT();
	~FocusFinderExecutorT();

	std::shared_ptr<FocusFinderT> getAlogirthm() const;
	void setAlgorithm(std::shared_ptr<FocusFinderT> focusFinder);


	void start();
	void cancel();
	bool isRunning() const;

	void reset(); // TODO: Required?

private:
	std::shared_ptr<FocusFinderT> mAlgorithm;

	std::thread mFocusFinderExecThread;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_EXECUTOR_H_ */

