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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_CURVE_GAUSSIAN_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_CURVE_GAUSSIAN_H_

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "curve_fit_exception.h"
#include "gsl_multi_fit_parms.h"

#include "lm_fitting_curve.h"

#include "enum_helper.h"


class LmFittingCurveGaussianT : public LmFittingCurveT {

public:
    struct IdxT {
        enum TypeE {
            B_IDX = 0, P_IDX, C_IDX, W_IDX, _Count
        };

        static const char *asStr(const TypeE &inType) {
            switch (inType) {
                case B_IDX:
                    return "B_IDX";
                case P_IDX:
                    return "P_IDX";
                case C_IDX:
                    return "C_IDX";
                case W_IDX:
                    return "W_IDX";
                default:
                    return "<?>";
            }
        }

        MAC_AS_TYPE(Type, E, _Count);
    };

    virtual ~LmFittingCurveGaussianT() = default;

    float fx(float x, const gsl_vector *curveParms) const override;

    [[nodiscard]] std::string getName() const override;

    [[nodiscard]] std::vector<std::string> getParmNames() const override;

    void makeGuess(const GslMultiFitParmsT &inData, gsl_vector *guess) override;

    int gslFx(const gsl_vector *curveParms, const GslMultiFitParmsT *gslParms, gsl_vector *outResultVec) override;

    int gslDfx(const gsl_vector *x, const GslMultiFitParmsT *gslParms, gsl_matrix *J) override;

    int gslFdfx(const gsl_vector *x, const GslMultiFitParmsT *gslParms, gsl_vector *f, gsl_matrix *J) override;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_LM_FITTING_CURVE_HYPERBOLIC_H_*/
