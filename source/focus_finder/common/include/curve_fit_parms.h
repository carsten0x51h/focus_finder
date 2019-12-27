#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CURVE_FIT_PARMS_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_CURVE_FIT_PARMS_H_

#include <boost/property_tree/ptree.hpp>

#include "fitting_curve_type.h"

class CurveFitParmsT {
public:
  CurveFitParmsT(FittingCurveTypeT::TypeE fittingCurveType = FittingCurveTypeT::_Count, float maxAcceptedRelativeError = 0.1F, float maxAcceptedAbsoluteError = 0.1F,
			       std::size_t numMaxIterations = 100, bool enableOutlierDetection = true, float outlierBoundaryFactor = 1.5F,
		float maxAcceptedOutliersPerc = 10.0F);

  FittingCurveTypeT::TypeE getFittingCurveType() const;
  void setFittingCurveType(FittingCurveTypeT::TypeE fittingCurveType);

  float getMaxAcceptedRelativError() const;
  void setMaxAcceptedRelativError(float maxAcceptedRelativeError);
  
  float getMaxAcceptedAbsoluteError() const;
  void setMaxAcceptedAbsoluteError(float maxAcceptedAbsoluteError);
  
  size_t getNumMaxIterations() const;
  void setNumMaxIterations(size_t numMaxIterations);

  bool getEnableOutlierDetection() const;
  void setEnableOutlierDetection(bool enableOutlierDetection);
  
  float getOutlierBoundaryFactor() const;
  void setOutlierBoundaryFactor(float outlierBoundaryFactor);
  
  float getMaxAcceptedOutliersPerc() const;
  void setMaxAcceptedOutliersPerc(float maxAcceptedOutliersPerc);


  static const CurveFitParmsT & defaults();
  static const CurveFitParmsT sDefaultCurveFitParms;
  
  
  // TODO: To avoid direct dependency of this data model to boost tree, move the load/save functions out of this class into a sep., standalone header.  
  static void save(boost::property_tree::ptree & pt, const CurveFitParmsT & curveFitParms);
  static CurveFitParmsT load(const boost::property_tree::ptree & pt);
  
  std::ostream & print(std::ostream & os, size_t indent = 0) const;

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
