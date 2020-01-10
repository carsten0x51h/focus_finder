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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_FITTING_CURVE_FACTORY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_FITTING_CURVE_FACTORY_H_

#include <memory>

#include "fitting_curve_type.h"
#include "lm_fitting_curve.h"


class LmFittingCurveFactoryT {
public:
	static std::shared_ptr<LmFittingCurveT> getInstance(const FittingCurveTypeT::TypeE & type);
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FITTING_CURVE_FACTORY_H_*/
