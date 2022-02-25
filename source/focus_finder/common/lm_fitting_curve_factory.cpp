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

#include "include/lm_fitting_curve.h"
#include "include/lm_fitting_curve_factory.h"
#include "include/lm_fitting_curve_hyperbolic.h"
#include "include/lm_fitting_curve_parabolic.h"
#include "include/lm_fitting_curve_gaussian.h"
#include "include/lm_fitting_curve_hyperbolic_pos_only.h"

/**
 * NOTE / TODO: In case the "LmFittingCurveHyperbolicPosOnlyT" is really required AND it should be constructed by this factory, we could supply parameters "a" and "b" using variadic templates. See the following link for an example:
 * https://stackoverflow.com/questions/28256550/factory-pattern-using-variadic-template
 */
std::shared_ptr<LmFittingCurveT> LmFittingCurveFactoryT::getInstance(const FittingCurveTypeT::TypeE &fittingCurveType) {

    switch (fittingCurveType) {
        case FittingCurveTypeT::HYPERBOLIC:
            return std::make_shared<LmFittingCurveHyperbolicT>();

        case FittingCurveTypeT::LINEAR_SLOPE:
            return nullptr; // TODO: Implement

        case FittingCurveTypeT::PARABOLIC:
            return std::make_shared<LmFittingCurveParabolicT>();

        case FittingCurveTypeT::GAUSSIAN:
            return std::make_shared<LmFittingCurveGaussianT>();

        case FittingCurveTypeT::HYPERBOLIC_POS_ONLY:
            return std::make_shared<LmFittingCurveHyperbolicPosOnlyT>();

        default:
            return nullptr;
    }
}
