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

/**
 * TODO: FwhMT class extension, Noise reduction, hot pixel filter(?),
 *
 * Use of non-linear-curve fitting to calculaze parameters of the gaussian curve.
 * We use the gsl (GNU scientific library) to do the curve fitting with the
 * Marquart-Levenberg algorithm.
 *
 * See http://en.wikipedia.org/wiki/Levenberg%E2%80%93Marquardt_algorithm
 *
 * Additional notes:
 *
 *  http://www.astro-imaging.com/Tutorial/MatchingCCD.html --> FWHM = 1.02 * (wavelength) * (Focal Ratio)
 *  http://www.stanmooreastro.com/pixel_size.htm
 *  --> As theory predicts, there is actually slight resolution degradation when FWHM = 3.0 pixels and significant degradation when FWHM = 1.5 pixels.
 *  --> As discussed above, typical amateur equipment and conditions can be expected to produce images with FWHM between 2.0 and 3.5 arcsec.
 *  --> Thus a pixel size of 0.5 to 1.5 arcsec can be considered ideal for optimizing resolution.
 *
 *  This is the full width of the image at half maximum value, or full-width half-maximum, or FWHM.
 *  It is a simple and well-defined number which can be used to compare the quality of images obtained
 *  under different observing conditions. In the usual sort of astronomical image, the FWHM is measured
 *  for a selection of stars in the frame and the "seeing" or image quality is reported as the mean value.
 *
 */

#ifndef _FWHM_H_
#define _FWHM_H_ _FWHM_H_

#include <vector>
#include <math.h>
#include <iostream>

#include "size.h"
#include "exception.h"
#include "curve_fit_algorithm.h"
#include "point.h"
#include "point_with_residual.h"
#include "curve_parms.h"

DEF_Exception(Fwhm);


// TODO: Make this a template!
class FwhmT {

private:
  bool mValid;
  std::vector<PointFT> mImgValues;
  std::vector<PointFT> mFitValues;
  std::vector<PointWithResidualT> mOutlierValues;
  
  CurveParmsT mGaussParms;
  
  static const double SIGMA_TO_FWHM;

  CurveParmsT fitValues(const std::vector<PointFT> & imgValues,
			double inEpsAbs = 1e-2, double inEpsRel = 1e-2);
  void calcIsValid();
  void throwIfNotValid() const;

public:
  FwhmT();
  FwhmT(const std::vector<float> & inValues, double inEpsAbs = 1e-2,
	     double inEpsRel = 1e-2, bool inThrowIfNotValid = true);
  void set(const std::vector<float> & inValues, double inEpsAbs = 1e-2,
	   double inEpsRel = 1e-2, bool inThrowIfNotValid = true);

  bool valid() const;
  void reset();

  /**
   * Calc FWHM ["] from FWHM [px]
   * FWHM = sigma * 1.66510922 (=2*sqrt(ln(2)))
   *
   * See http://mathworld.wolfram.com/GaussianFunction
   * TODO: --> Needs F?! --> Move out of here?!--> Diff class?!
   */
  //TODO: We want to have this value in PX and ".... depending on the purpose...
  
  static inline double sigmaToFwhm(double sigma) {
    return FwhmT::SIGMA_TO_FWHM * sigma;
  }
  static inline double fwhmToSigma(double sigma) {
    return sigma / FwhmT::SIGMA_TO_FWHM;
  }

  float getValue(bool inThrowIfNotValid = true) const;
  const std::vector<PointFT> & getImgValues() const;
  const std::vector<PointFT> & getFitValues() const;
  const std::vector<PointWithResidualT> & getOutlierValues() const;

  float getStandardDeviation() const;

  std::ostream & print(std::ostream & os, bool inPrintDetails = false) const;
  friend std::ostream & operator<<(std::ostream & os, const FwhmT & inFwhm);
};

#endif // _FWHM_HPP_
