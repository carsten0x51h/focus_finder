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

#include <ostream>
#include <boost/property_tree/ptree.hpp>

#include "include/fitting_curve_type.h"
#include "include/curve_fit_parms.h"


// TODO: Move to sep. class / file together with load / save...
#include "include/fitting_curve_type_ptree_translator.h"

const std::string CurveFitParmsT::sFittingCurveMatchingPath = "profile.settings.calibration_settings.curve_matching";


const CurveFitParmsT CurveFitParmsT::sDefaultCurveFitParms;

const CurveFitParmsT &CurveFitParmsT::defaults() {
    return sDefaultCurveFitParms;
}


CurveFitParmsT::CurveFitParmsT(FittingCurveTypeT::TypeE fittingCurveType, float maxAcceptedRelativeError,
                               float maxAcceptedAbsoluteError, size_t numMaxIterations, bool enableOutlierDetection,
                               float outlierBoundaryFactor, float maxAcceptedOutliersPerc) :

        mFittingCurveType(fittingCurveType), mMaxAcceptedRelativeError(maxAcceptedRelativeError),
        mMaxAcceptedAbsoluteError(maxAcceptedAbsoluteError), mNumMaxIterations(numMaxIterations),
        mEnableOutlierDetection(enableOutlierDetection), mOutlierBoundaryFactor(outlierBoundaryFactor),
        mMaxAcceptedOutliersPerc(maxAcceptedOutliersPerc) {

}

FittingCurveTypeT::TypeE CurveFitParmsT::getFittingCurveType() const {

    std::cerr << mFittingCurveType << std::endl;
    return mFittingCurveType;
}

void CurveFitParmsT::setFittingCurveType(FittingCurveTypeT::TypeE fittingCurveType) {
    mFittingCurveType = fittingCurveType;
}

float CurveFitParmsT::getMaxAcceptedRelativError() const {
    return mMaxAcceptedRelativeError;
}

void CurveFitParmsT::setMaxAcceptedRelativError(float maxAcceptedRelativeError) {
    mMaxAcceptedRelativeError = maxAcceptedRelativeError;
}

float CurveFitParmsT::getMaxAcceptedAbsoluteError() const {
    return mMaxAcceptedAbsoluteError;
}

void CurveFitParmsT::setMaxAcceptedAbsoluteError(float maxAcceptedAbsoluteError) {
    mMaxAcceptedAbsoluteError = maxAcceptedAbsoluteError;
}

size_t CurveFitParmsT::getNumMaxIterations() const {
    return mNumMaxIterations;
}

void CurveFitParmsT::setNumMaxIterations(size_t numMaxIterations) {
    mNumMaxIterations = numMaxIterations;
}

bool CurveFitParmsT::getEnableOutlierDetection() const {
    return mEnableOutlierDetection;
}

void CurveFitParmsT::setEnableOutlierDetection(bool enableOutlierDetection) {
    mEnableOutlierDetection = enableOutlierDetection;
}

float CurveFitParmsT::getOutlierBoundaryFactor() const {
    return mOutlierBoundaryFactor;
}

void CurveFitParmsT::setOutlierBoundaryFactor(float outlierBoundaryFactor) {
    mOutlierBoundaryFactor = outlierBoundaryFactor;
}

float CurveFitParmsT::getMaxAcceptedOutliersPerc() const {
    return mMaxAcceptedOutliersPerc;
}

void CurveFitParmsT::setMaxAcceptedOutliersPerc(float maxAcceptedOutliersPerc) {
    mMaxAcceptedOutliersPerc = maxAcceptedOutliersPerc;
}


// TODO: To avoid direct dependency of this data model to boost tree, move the load/save functions out of this class into a sep., standalone header.
void CurveFitParmsT::save(boost::property_tree::ptree &pt, const CurveFitParmsT &curveFitParms) {

    pt.put<typename FittingCurveTypeT::TypeE>(sFittingCurveMatchingPath + ".fitting_curve_type",
                                              curveFitParms.getFittingCurveType());
    pt.put<float>(sFittingCurveMatchingPath + ".max_relative_error", curveFitParms.getMaxAcceptedRelativError());
    pt.put<float>(sFittingCurveMatchingPath + ".max_absolute_error", curveFitParms.getMaxAcceptedAbsoluteError());
    pt.put<size_t>(sFittingCurveMatchingPath + ".num_max_iterations", curveFitParms.getNumMaxIterations());
    pt.put<bool>(sFittingCurveMatchingPath + ".enable_outlier_detection", curveFitParms.getEnableOutlierDetection());
    pt.put<float>(sFittingCurveMatchingPath + ".outlier_boundary_factor", curveFitParms.getOutlierBoundaryFactor());
    pt.put<float>(sFittingCurveMatchingPath + ".max_accepted_outliers_perc",
                  curveFitParms.getMaxAcceptedOutliersPerc());
}

CurveFitParmsT CurveFitParmsT::load(const boost::property_tree::ptree &pt) {
    CurveFitParmsT curveFitParms;

    curveFitParms.setFittingCurveType(
            pt.get<typename FittingCurveTypeT::TypeE>(sFittingCurveMatchingPath + ".fitting_curve_type"));

    curveFitParms.setMaxAcceptedRelativError(
            pt.get(sFittingCurveMatchingPath + ".max_relative_error", defaults().getMaxAcceptedRelativError()));
    curveFitParms.setMaxAcceptedAbsoluteError(
            pt.get(sFittingCurveMatchingPath + ".max_absolute_error", defaults().getMaxAcceptedAbsoluteError()));
    curveFitParms.setNumMaxIterations(
            pt.get(sFittingCurveMatchingPath + ".num_max_iterations", defaults().getNumMaxIterations()));
    curveFitParms.setEnableOutlierDetection(
            pt.get(sFittingCurveMatchingPath + ".enable_outlier_detection", defaults().getEnableOutlierDetection()));
    curveFitParms.setOutlierBoundaryFactor(
            pt.get(sFittingCurveMatchingPath + ".outlier_boundary_factor", defaults().getOutlierBoundaryFactor()));
    curveFitParms.setMaxAcceptedOutliersPerc(
            pt.get(sFittingCurveMatchingPath + ".max_accepted_outliers_perc", defaults().getMaxAcceptedOutliersPerc()));

    return curveFitParms;
}

std::ostream &CurveFitParmsT::print(std::ostream &os, size_t indent) const {
    std::string prefix = std::string(indent, ' ');

    os << prefix << "Fitting curve type: " << FittingCurveTypeT::asStr(mFittingCurveType) << std::endl
       << prefix << "Max accepted relative error: " << mMaxAcceptedRelativeError << std::endl
       << prefix << "Max accepted absolute error: " << mMaxAcceptedAbsoluteError << std::endl
       << prefix << "#MaxIterations: " << mNumMaxIterations << std::endl
       << prefix << "Outlier detection is: " << (mEnableOutlierDetection ? "enabled" : "disabled") << std::endl;

    if (mEnableOutlierDetection) {
        os << prefix << "outlierBoundaryFactor: " << mOutlierBoundaryFactor << std::endl
           << prefix << "maxAcceptedOutliersPerc: " << mMaxAcceptedOutliersPerc << std::endl;
    }
    return os;
}


std::ostream &operator<<(std::ostream &os,
                         const CurveFitParmsT &curveFitParms) {

    return curveFitParms.print(os);
}
