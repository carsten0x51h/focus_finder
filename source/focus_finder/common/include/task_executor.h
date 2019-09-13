/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TASK_EXECUTOR_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TASK_EXECUTOR_H_

#include <memory>
#include <thread>

#include "logging.h"

template<typename Task>
class TaskExecutorT {
private:
	// Prevent copy of TaskExecutor
	TaskExecutorT(const TaskExecutorT &);
	TaskExecutorT & operator=(const TaskExecutorT &);

public:
  TaskExecutorT() : mTask(nullptr) {
    LOG(debug) << "TaskExecutorT::TaskExecutorT..." << std::endl;
    
    reset();
  }
  ~TaskExecutorT() {
    LOG(debug) << "TaskExecutorT::~TaskExecutorT..." << std::endl;
  }

  std::shared_ptr<Task> getTask() const {
    return mTask;
  }

  void setTask(std::shared_ptr<Task> task) {
	// TODO: Check that not currently running - else throw exc....
	mTask = task;
  }

  void start() {
    // TODO: Check that it is not yet running.... otherwise throw? Or ignore?
    LOG(debug) << "TaskExecutorT::start..." << std::endl;

    mTaskExecThread = std::thread(&Task::run, mTask);
    mTaskExecThread.detach();
  }

  void cancel() {
    LOG(debug) << "TaskExecutorT::cancel..." << std::endl;
    
    if (mTask != nullptr) {
      mTask->cancel();
    }
  }

  bool isRunning() const {
    return (mTask ? mTask->isRunning() : false);
  }

  void reset() {
    // TODO
    LOG(debug) << "TaskExecutorT::reset..." << std::endl;

    mTask = nullptr;
  }

  
private:
  std::shared_ptr<Task> mTask;
  std::thread mTaskExecThread;
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TASK_EXECUTOR_H_ */
