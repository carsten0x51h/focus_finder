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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_FITTING_CURVE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_FITTING_CURVE_H_

#include <string>
#include <vector>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "gsl_multi_fit_parms.h"

#include "curve_parms.h"

class LmFittingCurveT {
public:
    virtual std::string getName() const = 0;

    //virtual std::vector<std::string> getParmNames() const = 0;
    template<typename Idx>
    std::vector<std::string>
    getParmNames() const {
        std::vector<std::string> parmNames(Idx::_Count);

        for (size_t idx = 0; idx < Idx::_Count; ++idx) {
            typename Idx::TypeE t = static_cast<typename Idx::TypeE>(idx);
            parmNames[idx] = Idx::asStr(t);
        }

        return parmNames;
    }

    // Those static wrappers are required since a pointer to them is passed to the C API of GSL.
    static int gslFx(const gsl_vector *curveParms, void *parms, gsl_vector *outResultVec);

    static int gslDfx(const gsl_vector *x, void *parms, gsl_matrix *J);

    static int gslFdfx(const gsl_vector *x, void *parms, gsl_vector *f, gsl_matrix *J);

    virtual std::vector<std::string> getParmNames() const = 0;


    float fx(float x, const CurveParmsT &curveParms) const;

    virtual float fx(float x, const gsl_vector *curveParms) const = 0;

    virtual void makeGuess(const GslMultiFitParmsT &inData, gsl_vector *guess) = 0;

    virtual int gslFx(const gsl_vector *x, const GslMultiFitParmsT *gslParams, gsl_vector *outResultVec) = 0;

    virtual int gslDfx(const gsl_vector *x, const GslMultiFitParmsT *gslParams, gsl_matrix *J) = 0;

    virtual int gslFdfx(const gsl_vector *x, const GslMultiFitParmsT *gslParams, gsl_vector *f, gsl_matrix *J) = 0;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_LM_FITTING_CURVE_H_*/
