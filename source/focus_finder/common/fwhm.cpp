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

#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
#include <vector>

#include "include/fwhm.h"
#include "include/throw_if.h"
#include "include/point.h"
#include "include/point_with_residual.h"
#include "include/fitting_curve_type.h"


const double FwhmT::SIGMA_TO_FWHM = 1.66510922; // 2.0 * sqrt(ln(2.0))

std::ostream &operator<<(std::ostream &os, const FwhmT &inFwhm) {
    return inFwhm.print(os);
}

FwhmT::FwhmT() : mValid(false) {
}

// TODO: Fwhm as template ??
FwhmT::FwhmT(const std::vector<float> &inValues, FwhmFitParmsT fwhmFitParms, bool inThrowIfNotValid) : mValid(false) {
    this->set(inValues, fwhmFitParms, inThrowIfNotValid);
}

void FwhmT::set(const std::vector<float> &inValues, FwhmFitParmsT fwhmFitParms, bool inThrowIfNotValid) {
    // Make a copy
    mImgValues.reserve(inValues.size());
    boost::range::copy(inValues | boost::adaptors::indexed(0) |
                       boost::adaptors::transformed([](const auto &e) { return PointFT(e.index(), e.value()); }),
                       std::back_inserter(mImgValues));

    mGaussParms = fitValues(mImgValues, fwhmFitParms);

    calcIsValid();

    if (inThrowIfNotValid) {
        throwIfNotValid();
    }
}

void FwhmT::throwIfNotValid() const {
    if (!mValid) {
        float sigma = mGaussParms.get("W_IDX").getValue();
        std::stringstream ss;
        ss << "FWHM calculated but is not valid. sigma=" << sigma << ", #data points=" << mImgValues.size()
           << ", #fit values=" << mFitValues.size() << std::endl;

        throw FwhmExceptionT(ss.str());
    }
}

CurveParmsT
FwhmT::fitValues(const std::vector<PointFT> &imgValues, FwhmFitParmsT fwhmFitParms) {

    mFitValues.reserve(imgValues.size());

    CurveParmsT curveParms;
    CurveFitSummaryT curveFitSummary;

    try {
        CurveFitParmsT curveFitParms(
                FittingCurveTypeT::GAUSSIAN,
                fwhmFitParms.getMaxAcceptedRelativeError(),
                fwhmFitParms.getMaxAcceptedAbsoluteError(),
                fwhmFitParms.getNumMaxIterations(),
                fwhmFitParms.isOutlierDetectionEnabled(),
                fwhmFitParms.getOutlierBoundaryFactor(),
                fwhmFitParms.getMaxAcceptedOutliersPerc()
        );

        curveParms = CurveFitAlgorithmT::fitCurve(imgValues,
                                                  curveFitParms,
                                                  &curveFitSummary);

        // Transfer over the data to the global storage. Plot the true points onto the graph as well.
        //*outFitValues = curveFitSummary.curveDataPoints; -> only y values? -> fwhm is different han focus curve...
        //boost::range::copy(curveFitSummary.curveDataPoints | boost::adaptors::transformed([](const auto & p){ return p.y(); }), std::back_inserter(*outFitValues));

        // Make a copy of values which were matched
        mFitValues = curveFitSummary.curveDataPoints;

        // Make a copy of outliers
        mOutlierValues = curveFitSummary.outliers;
    }
    catch (CurveFitExceptionT &exc) {
        // throw FwhmExceptionT - because the outer world does not know anything about the curve fitting...
        std::stringstream ss;
        ss << "Unable to fit values to gaussian curve: " << exc.what() << std::endl;
        throw FwhmExceptionT(ss.str());
    }

    return curveParms;
}

// TODO: Change to starmath::metrics::fwhm() -> FwhmResultT
float FwhmT::getValue(bool inThrowIfNotValid) const {
    if (inThrowIfNotValid) {
        throwIfNotValid();
    }

    return (float) sigmaToFwhm(mGaussParms.get("W_IDX").getValue());
}

CurveParmsT FwhmT::getCurveParms() const {
    return mGaussParms;
}

const std::vector<PointFT> &FwhmT::getImgValues() const {
    return mImgValues;
}

const std::vector<PointFT> &FwhmT::getFitValues() const {
    return mFitValues;
}

const std::vector<PointWithResidualT> &FwhmT::getOutlierValues() const {
    return mOutlierValues;
}

/**
 * Method to calculate the standrard deviation from values on the curve
 * to values measured values (image values).
 *
 * See http://de.wikipedia.org/wiki/Mittlere_quadratische_Abweichung
 */
float FwhmT::getStandardDeviation() const {
    throwIfNotValid();

    float mse = 0.0F;

    for (const auto &v : mFitValues) {
        float yFit = v.y();
        float yImg = mImgValues[v.x()].y();
        mse += std::pow(yFit - yImg, 2.0F);
    }

    return std::sqrt(mse / ((float) mFitValues.size() - 1.0F));
}

void FwhmT::calcIsValid() {
    float sigma = mGaussParms.get("W_IDX").getValue();
    bool calculatedSigmaNotValid = (sigma > mImgValues.size() || sigma <= 0);

    mValid = (!calculatedSigmaNotValid && ! mImgValues.empty() && ! mFitValues.empty());
}

bool FwhmT::valid() const {
    return mValid;
}

void FwhmT::reset() {
    mImgValues.clear();
    mFitValues.clear();
    mOutlierValues.clear();
    mValid = false;
}

std::ostream &
FwhmT::print(std::ostream &os, bool inPrintDetails) const {

    os << "FWHM=";

    if (mValid) {
        os << this->getValue() << "\"";
    } else {
        os << "<not valid>";
    }

    os << std::endl << "CurveParms: " << std::endl
       << mGaussParms;

    if (inPrintDetails) {

        os << std::endl << "   Img values: ";
        for (auto it = mImgValues.begin();
             it != mImgValues.end(); ++it) {
            os << *it << "; ";
        }
        os << ", Fit values: ";
        for (auto it = mFitValues.begin();
             it != mFitValues.end(); ++it) {
            os << *it << "; ";
        }
        os << ", Outliers: ";
        for (auto it = mOutlierValues.begin();
             it != mOutlierValues.end(); ++it) {
            os << *it << "; ";
        }
        os << ", std dev: " << getStandardDeviation() << std::endl;
    }

    return os;
}
