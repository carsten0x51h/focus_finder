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

#include <gsl/gsl_vector.h>

#include "include/lm_curve_matcher.h"
#include "include/lm_fitting_curve.h"

LmCurveMatcherT::LmCurveMatcherT(std::shared_ptr<LmFittingCurveT> lmFittingCurve) :
        mLmFittingCurve(lmFittingCurve) {

}

CurveParmsT LmCurveMatcherT::fillCurveParms(
        const gsl_vector *resultVec) const {
    CurveParmsT curveParms(resultVec->size);
    auto parmNames = mLmFittingCurve->getParmNames();

    //TODO: FIXED_PARMS - Only iterate about  the non-fixed ones... --> ask mLmFittingCurve for this info...

    for (size_t idx = 0; idx < resultVec->size; ++idx) {
        std::string parmName = parmNames.at(idx);
        float value = gsl_vector_get(resultVec, idx);

        curveParms[idx] = CurveParmT(parmName, value);
    }

    return curveParms;
}
