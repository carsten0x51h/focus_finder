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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_VALIDATION_EXCEPTION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_VALIDATION_EXCEPTION_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_VALIDATION_EXCEPTION_H_

#include <string>
#include <exception>
#include <utility>

// TODO: Maybe later add details about the validation problem...
class ValidationExceptionT : public std::exception {
public:
    explicit ValidationExceptionT(std::string inMsg = "") :
            mName("ValidationExceptionT"), mMsg(std::move(inMsg)) {
    }

    ~ValidationExceptionT() noexcept override = default;

    [[nodiscard]] const char *what() const noexcept override {
        return mMsg.c_str();
    }

    [[nodiscard]] const char *getName() const noexcept {
        return mName.c_str();
    }

private:
    std::string mName;
    std::string mMsg;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_VALIDATION_EXCEPTION_H_*/
