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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CALIBRATION_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CALIBRATION_H_

#include <boost/property_tree/ptree.hpp>

#include "curve_fit_parms.h"
#include "focus_curve_record_set.h"
#include "focus_curve_record_set_container.h"
#include "focus_finder_calibration.h"
#include "focus_curve.h"

class FocusFinderCalibrationT {

 public:
  FocusFinderCalibrationT();
  ~FocusFinderCalibrationT();
  
  FocusCurveRecordSetContainerT & getCalibrationData();
  void setCalibrationData(const FocusCurveRecordSetContainerT & focusCurveRecordSetContainer, const CurveFitParmsT & curveFitParms);

  bool isValid() const;
  std::shared_ptr<FocusCurveT> getFocusCurve() const;
  size_t numRecordSets() const;
  
  static void save(boost::property_tree::ptree & pt, std::shared_ptr<FocusFinderCalibrationT> focusFinderCalibration);
  static std::shared_ptr<FocusFinderCalibrationT> load(const boost::property_tree::ptree & pt, const CurveFitParmsT & curveFitParms);

  std::ostream & print(std::ostream & os, size_t indent = 0) const;




 private:
  
  /**
   * Calibration settings used when this calibration was recorded.
   * -> we do not save a copy... because the calibration in any case
   *    must fit the rest of the profile (eve, the devices etc.).
   */
  //  FocusFinderCalibrationSettingsT mFocusFinderCalibrationSettings;

  /**
   * TODO: FocusCurve / funcion / parameters / boundaries....
   */


  /**
   * Focus curve calculated from the calibration data.
   * In case there is no data (mFocusCurveRecordSetContainer empty)
   * or not sufficient data or a problem occurs during calculation of
   * the focus curve, the ptr is nullptr.
   *
   * Question is, if this should not be calculated always when loading based
   * on "mFocusCurveRecordSetContainer"... However, it could still be a member
   * of FocusFinderCalibrationT. Then we would have to store at least
   * "FittingCurveTypeT::TypeE curveType". Then it would be possible to use
   * the constructor
   * "FocusCurveT(std::shared_ptr<const FocusCurveRecordSetT> focusCurveRecordSet, FittingCurveTypeT::TypeE curveType)".
   * That would have the advantage, that all the results of the curve fitting would be
   * available after the calculation and would not have to be stored! Question is,
   * if the LM algorithm is deterministic. Fast enough it is...
   * Another advantage is, that when editing a "calibration" (the data points),
   * only those need to be saved again. After the next loading, the FocusCurveT
   * is re-calculated automatically... 
   */
  std::shared_ptr<FocusCurveT> mFocusCurve;

  /**
   * Recorded calibration data.
   */
  FocusCurveRecordSetContainerT mFocusCurveRecordSetContainer;
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CALIBRATION_H_*/
