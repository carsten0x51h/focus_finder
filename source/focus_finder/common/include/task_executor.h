/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c.schmitt51h@gmail.com>
 *
 *  More info on https://www.lost-infinity.com
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

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

    TaskExecutorT &operator=(const TaskExecutorT &);

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
