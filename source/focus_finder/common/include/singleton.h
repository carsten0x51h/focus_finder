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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SINGLETON_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SINGLETON_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SINGLETON_H_

#include <utility>

template<class T>
class SingletonT {
public:
    template<typename ... Args>
    static T * getInstance(Args ... args) {
        if (!mInstance) {
            mInstance = new T(std::forward<Args>(args)...);
        }

        return mInstance;
    }

    static
    void destroyInstance() {
        delete mInstance;
        mInstance = nullptr;
    }

private:
    static T* mInstance;
};

template<class T> T* SingletonT<T>::mInstance = nullptr;

#endif //SOURCE_FOCUS_FINDER_COMMON_INCLUDE_SINGLETON_H_
