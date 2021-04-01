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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CURVE_FIT_PARMS_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CURVE_FIT_PARMS_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CURVE_FIT_PARMS_H_

#include <boost/property_tree/ptree.hpp>

#include "fitting_curve_type.h"

class CurveFitParmsT {
public:
    explicit CurveFitParmsT(FittingCurveTypeT::TypeE fittingCurveType = FittingCurveTypeT::_Count,
                   float maxAcceptedRelativeError = 0.1F, float maxAcceptedAbsoluteError = 0.1F,
                   std::size_t numMaxIterations = 100, bool enableOutlierDetection = true,
                   float outlierBoundaryFactor = 1.5F,
                   float maxAcceptedOutliersPerc = 10.0F);

    [[nodiscard]] FittingCurveTypeT::TypeE getFittingCurveType() const;

    void setFittingCurveType(FittingCurveTypeT::TypeE fittingCurveType);

    [[nodiscard]] float getMaxAcceptedRelativError() const;

    void setMaxAcceptedRelativError(float maxAcceptedRelativeError);

    [[nodiscard]] float getMaxAcceptedAbsoluteError() const;

    void setMaxAcceptedAbsoluteError(float maxAcceptedAbsoluteError);

    [[nodiscard]] size_t getNumMaxIterations() const;

    void setNumMaxIterations(size_t numMaxIterations);

    [[nodiscard]] bool getEnableOutlierDetection() const;

    void setEnableOutlierDetection(bool enableOutlierDetection);

    [[nodiscard]] float getOutlierBoundaryFactor() const;

    void setOutlierBoundaryFactor(float outlierBoundaryFactor);

    [[nodiscard]] float getMaxAcceptedOutliersPerc() const;

    void setMaxAcceptedOutliersPerc(float maxAcceptedOutliersPerc);


    static const CurveFitParmsT &defaults();

    static const CurveFitParmsT sDefaultCurveFitParms;


    // TODO: To avoid direct dependency of this data model to boost tree, move the load/save functions out of this class into a sep., standalone header.
    static void save(boost::property_tree::ptree &pt, const CurveFitParmsT &curveFitParms);

    static CurveFitParmsT load(const boost::property_tree::ptree &pt);

    std::ostream &print(std::ostream &os, size_t indent = 0) const;

    // TODO: Remove... member access no longer required since there is now a print() function...
    //  friend std::ostream & operator<<(std::ostream & os, const CurveFitParmsT & curveFitParms);


private:
    // TODO: Move to another file / class togehter with load / save...
    static const std::string sFittingCurveMatchingPath;


    FittingCurveTypeT::TypeE mFittingCurveType;
    float mMaxAcceptedRelativeError;
    float mMaxAcceptedAbsoluteError;
    size_t mNumMaxIterations;
    bool mEnableOutlierDetection;
    float mOutlierBoundaryFactor;
    float mMaxAcceptedOutliersPerc;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CURVE_FIT_PARMS_H_*/
