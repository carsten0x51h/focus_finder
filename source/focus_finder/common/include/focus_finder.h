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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_H_

#include <memory>
#include <functional>
#include <utility>

#include "exception.h"

class FocusCurveRecordT;
class FocusControllerT;

//DEF_Exception(FocusFinderCancelled);

class FocusFinderT {
private:
    typedef boost::signals2::signal<void()> FocusFinderStartedListenersT;
    FocusFinderStartedListenersT mFocusFinderStartedListeners;

    typedef boost::signals2::signal<void(float, std::string,
                                         std::shared_ptr<FocusCurveRecordT>)> FocusFinderProgressUpdateListenersT;
    FocusFinderProgressUpdateListenersT mFocusFinderProgressUpdateListeners;

    typedef boost::signals2::signal<void()> FocusFinderFinishedListenersT;
    FocusFinderFinishedListenersT mFocusFinderFinishedListeners;

    typedef boost::signals2::signal<void()> FocusFinderCancelledListenersT;
    FocusFinderCancelledListenersT mFocusFinderCancelledListeners;

    std::shared_ptr<FocusControllerT> mFocusController;

public:
    explicit FocusFinderT(std::shared_ptr<FocusControllerT> focusAnalyzer) : mFocusController(std::move(focusAnalyzer)) {
    }

    // Prevent copy of FocusFinder
    FocusFinderT(const FocusFinderT &) = delete;
    FocusFinderT &operator=(const FocusFinderT &) = delete;

    virtual ~FocusFinderT() = default;

    std::shared_ptr<FocusControllerT> getFocusController() { return mFocusController; }

    [[nodiscard]] virtual std::string getName() const = 0;

    [[nodiscard]] virtual bool isRunning() const = 0;

    virtual void run() = 0;

    virtual void cancel() = 0;

    virtual void reset() = 0;


    boost::signals2::connection registerFocusFinderStartedListener(
            const FocusFinderStartedListenersT::slot_type &inCallBack) {
        return mFocusFinderStartedListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterFocusFinderStartedListener(
            const T &inCallBack) {
        inCallBack.disconnect();
    }

    boost::signals2::connection registerFocusFinderProgressUpdateListener(
            const FocusFinderProgressUpdateListenersT::slot_type &inCallBack) {
        return mFocusFinderProgressUpdateListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterFocusFinderProgressUpdateListener(
            const T &inCallBack) {
        inCallBack.disconnect();
    }

    boost::signals2::connection registerFocusFinderFinishedListener(
            const FocusFinderFinishedListenersT::slot_type &inCallBack) {
        return mFocusFinderFinishedListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterFocusFinderFinishedListener(
            const T &inCallBack) {
        inCallBack.disconnect();
    }

    boost::signals2::connection registerFocusFinderCancelledListener(
            const FocusFinderCancelledListenersT::slot_type &inCallBack) {
        return mFocusFinderCancelledListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterFocusFinderCancelledListener(
            const T &inCallBack) {
        inCallBack.disconnect();
    }

    // TODO: Add Failed...

protected:
    void notifyFocusFinderStarted() const {
        mFocusFinderStartedListeners();
    }

    void notifyFocusFinderProgressUpdate(float progress,
                                         const std::string &msg,
                                         std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
        mFocusFinderProgressUpdateListeners(progress, msg, std::move(focusCurveRecord));
    }

    void notifyFocusFinderFinished() const {
        mFocusFinderFinishedListeners();
    }

    void notifyFocusFinderCancelled() const {
        mFocusFinderCancelledListeners();
    }
};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_H_ */
