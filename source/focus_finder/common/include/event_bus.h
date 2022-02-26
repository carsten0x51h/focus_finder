/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c [at] lost-infinity.com>
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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_EVENT_BUS_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_EVENT_BUS_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_EVENT_BUS_H_

#include <string>
#include <vector>

#include <boost/signals2.hpp>

#include "singleton.h"

class EventBusT : public SingletonT<EventBusT> {
public:
    EventBusT() = default;
    virtual ~EventBusT() = default;

    template<typename T>
    boost::signals2::signal<T>* createSignal() {
        typedef boost::signals2::signal<T> SignalT;

        if (mSignals.find(typeid(T).name()) == mSignals.end()) {
            auto * signal = new SignalT();
            mSignals[typeid(T).name()] = signal;
            return (signal);
        }

        return 0;
    }


    /**
     *
     * @tparam T
     * @param callback
     * @return
     */
    template<typename T>
    boost::signals2::connection subscribe(const boost::function<T>& callback) {
        typedef boost::signals2::signal<T> SignalT;
        SignalT* signal = nullptr;

        if (mSignals.find(typeid(T).name()) == mSignals.end()) {
            signal = createSignal<T>();
        } else {
            signal = dynamic_cast<SignalT*>(mSignals[typeid(T).name()]);
        }

        boost::signals2::connection ret = signal->connect(callback);

        mConnections[typeid(T).name()].push_back(ret);
        mSharedConnections[typeid(T).name()].push_back(
                boost::signals2::shared_connection_block(
                        mConnections[typeid(T).name()].back(), false));
//    signalsChanged ();
        return (ret);
    }


    /**
     * TODO: Implement unsubscribe...
     */



    /**
     *
     * @tparam T
     * @tparam Args
     * @param args
     */
    template<typename T, typename ... Args>
    void post(Args ... args) {
        typedef boost::signals2::signal<T> SignalT;
        if (mSignals.find(typeid(T).name()) != mSignals.end()) {
            SignalT* signal = dynamic_cast<SignalT*>(mSignals[typeid(T).name()]);
            signal->operator()(std::forward<Args>(args)...);
        }
    }

private:
    std::map<std::string, boost::signals2::signal_base*> mSignals;
    std::map<std::string, std::vector<boost::signals2::connection> > mConnections;
    std::map<std::string, std::vector<boost::signals2::shared_connection_block> > mSharedConnections;
};

#endif // SOURCE_FOCUS_FINDER_COMMON_INCLUDE_EVENT_BUS_H_
