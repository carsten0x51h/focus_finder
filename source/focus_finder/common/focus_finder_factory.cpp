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

#include <memory>

#include "include/focus_finder_factory.h"
#include "include/focus_finder_fast_curve_lookup.h"

class FocusControllerT;

std::shared_ptr<FocusFinderT> FocusFinderFactoryT::getInstance(
							       const FocusFinderStrategyT::TypeE & strategy, std::shared_ptr<FocusControllerT> focusAnalyzer) {

	switch (strategy) {
	case FocusFinderStrategyT::FAST_CURVE_LOOKUP:
		return std::make_shared<FocusFinderFastCurveLookupT>(focusAnalyzer);

	default:
		return nullptr;
	}
}
