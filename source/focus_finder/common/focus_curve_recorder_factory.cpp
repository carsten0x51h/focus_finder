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

#include <memory>

#include "include/focus_curve_recorder_factory.h"
#include "include/default_focus_curve_recorder.h"

class FocusControllerT;

std::shared_ptr<FocusCurveRecorderT> FocusCurveRecorderFactoryT::getInstance(
        const FocusCurveRecorderTypeT::TypeE &type, std::shared_ptr<FocusControllerT> focusAnalyzer) {

    switch (type) {
        case FocusCurveRecorderTypeT::DEFAULT:
            return std::make_shared<DefaultFocusCurveRecorderT>(focusAnalyzer);

        default:
            return nullptr;
    }
}
