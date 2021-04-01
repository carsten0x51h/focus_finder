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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_H_

#include <memory>
#include <functional>
#include <boost/signals2.hpp>
#include <utility>

#include "exception.h"

#include "focus_curve_record_set.h"
#include "focus_curve_record_set_container.h"
#include "focus_controller.h"

// DEF_Exception(FocusCurveRecorder);
// DEF_Exception(FocusCurveRecorderFailed);
// DEF_Exception(FocusCurveRecorderCancelled);

class FocusCurveRecordT;
class FocusCurveRecordSetT;

class FocusCurveRecorderT {
private:
    typedef boost::signals2::signal<void()> FocusCurveRecorderStartedListenersT;
    FocusCurveRecorderStartedListenersT mFocusCurveRecorderStartedListeners;

    typedef boost::signals2::signal<void(
            std::shared_ptr<FocusCurveRecordSetT>)> FocusCurveRecorderRecordSetFinishedListenersT;
    FocusCurveRecorderRecordSetFinishedListenersT mFocusCurveRecorderRecordSetFinishedListeners;

    typedef boost::signals2::signal<void(
            std::shared_ptr<const FocusCurveRecordSetContainerT>)> FocusCurveRecorderFinishedListenersT;
    FocusCurveRecorderFinishedListenersT mFocusCurveRecorderFinishedListeners;

    typedef boost::signals2::signal<void()> FocusCurveRecorderCancelledListenersT;
    FocusCurveRecorderCancelledListenersT mFocusCurveRecorderCancelledListeners;

    typedef boost::signals2::signal<void(std::shared_ptr<FocusCurveRecordT>)> FocusCurveRecorderNewRecordListenersT;
    FocusCurveRecorderNewRecordListenersT mFocusCurveRecorderNewRecordListeners;

    typedef boost::signals2::signal<void(
            std::shared_ptr<FocusCurveRecordSetT>)> FocusCurveRecorderRecordSetUpdateListenersT;
    FocusCurveRecorderRecordSetUpdateListenersT mFocusCurveRecorderRecordSetUpdateListeners;


    typedef boost::signals2::signal<void(float, std::string,
                                         std::shared_ptr<FocusCurveRecordT>)> FocusCurveRecorderProgressUpdateListenersT;
    FocusCurveRecorderProgressUpdateListenersT mFocusCurveRecorderProgressUpdateListeners;

    std::shared_ptr<FocusControllerT> mFocusController;

public:
    explicit FocusCurveRecorderT(std::shared_ptr<FocusControllerT> focusController) : mFocusController(std::move(focusController)) {
    }

    virtual ~FocusCurveRecorderT() = default;

    // Prevent copy of FocusCurveRecorder
    FocusCurveRecorderT(const FocusCurveRecorderT &) = delete;
    FocusCurveRecorderT &operator=(const FocusCurveRecorderT &) = delete;

    std::shared_ptr<FocusControllerT> getFocusController() { return mFocusController; }

    [[nodiscard]] std::shared_ptr<const FocusControllerT> getFocusController() const { return mFocusController; }

    [[nodiscard]] virtual std::string getName() const = 0;

    [[nodiscard]] virtual bool isRunning() const = 0;

    virtual void run() = 0;

    virtual void cancel() = 0;

    virtual void reset() = 0;

    [[nodiscard]] virtual std::shared_ptr<const FocusCurveRecordSetContainerT> getFocusCurveRecordSets() const = 0;


    boost::signals2::connection registerFocusCurveRecorderStartedListener(
            const FocusCurveRecorderStartedListenersT::slot_type &inCallBack) {
        return mFocusCurveRecorderStartedListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterFocusCurveRecorderStartedListener(
            const T &inCallBack) {
        inCallBack.disconnect();
    }


    boost::signals2::connection registerFocusCurveRecorderRecordSetFinishedListener(
            const FocusCurveRecorderRecordSetFinishedListenersT::slot_type &inCallBack) {
        return mFocusCurveRecorderRecordSetFinishedListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterFocusCurveRecorderRecordSetFinishedListener(
            const T &inCallBack) {
        inCallBack.disconnect();
    }


    boost::signals2::connection registerFocusCurveRecorderFinishedListener(
            const FocusCurveRecorderFinishedListenersT::slot_type &inCallBack) {
        return mFocusCurveRecorderFinishedListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterFocusCurveRecorderFinishedListener(
            const T &inCallBack) {
        inCallBack.disconnect();
    }

    boost::signals2::connection registerFocusCurveRecorderCancelledListener(
            const FocusCurveRecorderCancelledListenersT::slot_type &inCallBack) {
        return mFocusCurveRecorderCancelledListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterFocusCurveRecorderCancelledListener(
            const T &inCallBack) {
        inCallBack.disconnect();
    }


    boost::signals2::connection registerFocusCurveRecorderNewRecordListener(
            const FocusCurveRecorderNewRecordListenersT::slot_type &inCallBack) {
        return mFocusCurveRecorderNewRecordListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterFocusCurveRecorderNewRecordListener(
            const T &inCallBack) {
        inCallBack.disconnect();
    }

    boost::signals2::connection registerFocusCurveRecorderRecordSetUpdateListener(
            const FocusCurveRecorderRecordSetUpdateListenersT::slot_type &inCallBack) {
        return mFocusCurveRecorderRecordSetUpdateListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterFocusCurveRecorderRecordSetUpdateListener(
            const T &inCallBack) {
        inCallBack.disconnect();
    }


    boost::signals2::connection registerFocusCurveRecorderProgressUpdateListener(
            const FocusCurveRecorderProgressUpdateListenersT::slot_type &inCallBack) {
        return mFocusCurveRecorderProgressUpdateListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterFocusCurveRecorderProgressUpdateListener(
            const T &inCallBack) {
        inCallBack.disconnect();
    }


protected:
    void notifyFocusCurveRecorderStarted() const {
        // HACK: Does not compile...
        //mFocusCurveRecorderStartedListeners();
    }

    void notifyFocusCurveRecorderRecordSetFinished(std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet) const {
        mFocusCurveRecorderRecordSetFinishedListeners(std::move(focusCurveRecordSet));
    }

    void notifyFocusCurveRecorderFinished(
            std::shared_ptr<const FocusCurveRecordSetContainerT> focusCurveRecordSetContainer) const {
        mFocusCurveRecorderFinishedListeners(std::move(focusCurveRecordSetContainer));
    }

    void notifyFocusCurveRecorderCancelled() {
        mFocusCurveRecorderCancelledListeners();
    }

    void notifyFocusCurveRecorderNewRecord(std::shared_ptr<FocusCurveRecordT> focusCurveRecord) {
        mFocusCurveRecorderNewRecordListeners(std::move(focusCurveRecord));
    }

    void notifyFocusCurveRecorderRecordSetUpdate(std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet) {
        mFocusCurveRecorderRecordSetUpdateListeners(std::move(focusCurveRecordSet));
    }

    void notifyFocusCurveRecorderProgressUpdate(float progress,
                                                const std::string &msg,
                                                std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
        mFocusCurveRecorderProgressUpdateListeners(progress, msg, std::move(focusCurveRecord));
    }

    void notifyFocusCurveRecorderProgressUpdate(const std::string &msg,
                                                std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
        mFocusCurveRecorderProgressUpdateListeners(-1.0F, msg, std::move(focusCurveRecord));
    }

    void notifyFocusCurveRecorderProgressUpdate(std::shared_ptr<FocusCurveRecordT> focusCurveRecord = nullptr) const {
        mFocusCurveRecorderProgressUpdateListeners(-1.0F, "", std::move(focusCurveRecord));
    }

    // TODO: Add update...
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_CURVE_RECORDER_H_*/
