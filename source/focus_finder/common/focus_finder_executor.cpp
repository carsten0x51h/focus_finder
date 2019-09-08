#include <thread>

#include "include/logging.h"
#include "include/focus_finder.h"
#include "include/focus_finder_executor.h"

FocusFinderExecutorT::FocusFinderExecutorT() : mAlgorithm(nullptr) {
	LOG(debug) << "FocusFinderExecutorT::FocusFinderExecutorT..." << std::endl;

	reset();
}

FocusFinderExecutorT::~FocusFinderExecutorT() {
	LOG(debug) << "FocusFinderExecutorT::~FocusFinderExecutorT..." << std::endl;
}

std::shared_ptr<FocusFinderT> FocusFinderExecutorT::getAlogirthm() const {
	return mAlgorithm;
}

void FocusFinderExecutorT::setAlgorithm(std::shared_ptr<FocusFinderT> focusFinder) {
	// TODO: Check that not currently running - else throw exc....

	mAlgorithm = focusFinder;
}

void FocusFinderExecutorT::start() {
	// TODO: Check that it is not yet running.... otherwise torow? Or ignore?
	LOG(debug) << "FocusFinderExecutorT::start..." << std::endl;

	mFocusFinderExecThread = std::thread(&FocusFinderT::run, mAlgorithm);
	mFocusFinderExecThread.detach();
}

void FocusFinderExecutorT::cancel() {
	// TODO
	LOG(debug) << "FocusFinderExecutorT::cancel..." << std::endl;
	mAlgorithm->cancel();
}

bool FocusFinderExecutorT::isRunning() const {
    return (mAlgorithm ? mAlgorithm->isRunning() : false);
}

void FocusFinderExecutorT::reset() {
	// TODO
	LOG(debug) << "FocusFinderExecutorT::reset..." << std::endl;

	mAlgorithm = nullptr;
}
