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

#include <gsl/gsl_vector.h>

#include "include/lm_fitting_curve.h"
#include "include/lm_fitting_parms_container.h"
#include "include/curve_parms.h"

// static!
int LmFittingCurveT::gslFx(const gsl_vector *curveParms, void *parms, gsl_vector *outResultVec) {
    auto *parmsContainer = static_cast<LmFittingParmsContainerT *>(parms);
    return parmsContainer->obj->gslFx(curveParms, parmsContainer->gslParms, outResultVec);
}

// static!
int LmFittingCurveT::gslDfx(const gsl_vector *x, void *parms, gsl_matrix *J) {
    auto *parmsContainer = static_cast<LmFittingParmsContainerT *>(parms);
    return parmsContainer->obj->gslDfx(x, parmsContainer->gslParms, J);
}

// static!
int LmFittingCurveT::gslFdfx(const gsl_vector *x, void *parms, gsl_vector *f, gsl_matrix *J) {
    auto *parmsContainer = static_cast<LmFittingParmsContainerT *>(parms);
    return parmsContainer->obj->gslFdfx(x, parmsContainer->gslParms, f, J);
}


float LmFittingCurveT::fx(float x, const CurveParmsT &curveParms) const {

    gsl_vector *gslParms = gsl_vector_alloc(curveParms.size());

    for (size_t idx = 0; idx < curveParms.size(); ++idx) {
        float value = curveParms.get(idx).getValue();
        gsl_vector_set(gslParms, idx, value);
    }

    float y = this->fx(x, gslParms);

    gsl_vector_free(gslParms);

    return y;
}
