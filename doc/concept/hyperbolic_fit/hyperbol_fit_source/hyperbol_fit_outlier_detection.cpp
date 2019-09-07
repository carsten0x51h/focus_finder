#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <tuple> // For std::tie

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm.hpp>

#include <gsl/gsl_multifit_nlin.h>


/**********************************************************************
 * Helper classes
 **********************************************************************/
struct GslMultiFitDataT {
  float x;
  float y;
  float sigma;
  GslMultiFitDataT(float inX, float inY, float inSigma) : x(inX), y(inY), sigma(inSigma) { };
};
  
typedef std::vector<GslMultiFitDataT> GslMultiFitParmsT;


/**********************************************************************
 * Curve to fit to is supplied by traits.
 **********************************************************************/
  
/**
 * See http://en.wikipedia.org/wiki/Approximation_error for expl. of rel and abs errors.
 *
 * returns 0 in case of success, otherwise value != 0 (GSL error code).
 */
template <typename LMFitTraitsT, typename RngT>
static int
levenbergMarquartCurveFit(RngT rng, typename LMFitTraitsT::CurveParamsT::TypeT * outResults, double inEpsAbs, double inEpsRel, size_t inNumMaxIter = 5000) {

  size_t numPoints = std::distance(rng.begin(), rng.end());
    
  GslMultiFitParmsT gslMultiFitParms;
  gslMultiFitParms.reserve(numPoints);
    
  // Fill in the parameters
  for (auto it = rng.begin(); it != rng.end(); ++it) {
    gslMultiFitParms.push_back(GslMultiFitDataT(it->x(), it->y(), 0.1f));
  }
 
  // Fill in function info
  gsl_multifit_function_fdf f;
  f.f      = LMFitTraitsT::gslFx;
  f.df     = LMFitTraitsT::gslDfx;
  f.fdf    = LMFitTraitsT::gslFdfx;
  f.n      = numPoints;
  f.p      = LMFitTraitsT::CurveParamsT::_Count;
  f.params = & gslMultiFitParms;
    
 
  gsl_vector * guess = gsl_vector_alloc(LMFitTraitsT::CurveParamsT::_Count);  // Allocate the guess vector
    
  LMFitTraitsT::makeGuess(gslMultiFitParms, guess);  // Make initial guesses - here we just set all parameters to 1.0
    
  // Create a Levenberg-Marquardt solver with n data points and m parameters
  gsl_multifit_fdfsolver * solver = gsl_multifit_fdfsolver_alloc(gsl_multifit_fdfsolver_lmsder,
								 numPoints, LMFitTraitsT::CurveParamsT::_Count);
  gsl_multifit_fdfsolver_set(solver, & f, guess);  // Initialize the solver
    
  int status, i = 0;
    
  // Iterate to to find a result
  do {
    i++;
    status = gsl_multifit_fdfsolver_iterate(solver); // returns 0 in case of success
    if (status) {  break; }
    status = gsl_multifit_test_delta(solver->dx, solver->x, inEpsAbs, inEpsRel);
  } while (status == GSL_CONTINUE && i < inNumMaxIter);

  // Store the results to be returned to the user (copy from gsl_vector to result structure)
  for (size_t i = 0; i < LMFitTraitsT::CurveParamsT::_Count; ++i) {
    typename LMFitTraitsT::CurveParamsT::TypeE idx = static_cast<typename LMFitTraitsT::CurveParamsT::TypeE>(i);
    (*outResults)[idx] = gsl_vector_get(solver->x, idx);
  }
 
  // Free GSL memory
  gsl_multifit_fdfsolver_free(solver);
  gsl_vector_free(guess);
 
  return status;
}


/**********************************************************************
 * Hyperbol fit traits
 **********************************************************************/
class HyperbolFitTraitsT {
private:
  
public:
  struct CurveParamsT {
    enum TypeE { A_IDX = 0, B_IDX, C_IDX, D_IDX, _Count };
    struct TypeT : public std::array<float, TypeE::_Count> {
      TypeT(const gsl_vector * inVec = 0) {
        for (size_t i = 0; i < TypeE::_Count; ++i) {
          TypeE idx = static_cast<TypeE>(i);
          (*this)[i] = (inVec ? gsl_vector_get(inVec, idx) : 0);
        }
      }
    };
  };
 
  /**
   * All parameters with good starting values - may be improved.
   *
   * minmax See https://stackoverflow.com/questions/16772477/how-to-get-min-or-max-element-in-a-vector-of-structures-in-c-based-on-some-fi
   */
  static void makeGuess(const GslMultiFitParmsT & inData, gsl_vector * guess) {
    // Find min HFD -> good start value for c
    GslMultiFitParmsT::const_iterator minEl, maxEl;
    std::tie(minEl, maxEl) = std::minmax_element(begin(inData), end(inData),
						 [] (GslMultiFitDataT const& s1, GslMultiFitDataT const& s2) {
						   return s1.y < s2.y;
						 });
    
    gsl_vector_set(guess, CurveParamsT::A_IDX, 1.0);
    gsl_vector_set(guess, CurveParamsT::B_IDX, 2.0);
    gsl_vector_set(guess, CurveParamsT::C_IDX, minEl->x);
    gsl_vector_set(guess, CurveParamsT::D_IDX, 1.0);
  }

  /**
   * Phi() is a repeating part of the function calculation used by
   * different derivations.
   */
  static float phi(float x, float a, float c) {
    const float a2 = a * a;
    const float diff = x - c;
    const float diff2 = diff * diff;
    
    return sqrt(1.0 + (diff2 / a2));
  }
  
  /* H(x) */
  static float fx(float x, const CurveParamsT::TypeT & inParms) {
    const float a = inParms[CurveParamsT::A_IDX];
    const float b = inParms[CurveParamsT::B_IDX];
    const float c = inParms[CurveParamsT::C_IDX];
    const float d = inParms[CurveParamsT::D_IDX];
    
    return b * phi(x, a, c) + d;
  }
 
  /* Calculates H(x) for each data point. */
  static int gslFx(const gsl_vector * x, void * inGslParams, gsl_vector * outResultVec) {    
    // Store the current coefficient values
    CurveParamsT::TypeT curveParams(x);
    
    // Store parameter values
    const GslMultiFitParmsT * gslParams = ((GslMultiFitParmsT*) inGslParams); 
    // Execute Levenberg-Marquart on f(x)
    for(size_t i = 0; i < gslParams->size(); ++i) {
      const GslMultiFitDataT & gslData = gslParams->at(i);
      float yi = HyperbolFitTraitsT::fx((float) gslData.x, curveParams);

      // TODO: Is this ok?
      gsl_vector_set(outResultVec, i, (yi - gslData.y) / gslData.sigma);
    }
    
    return GSL_SUCCESS;
  }
 
  /* Calculates the Jacobian (derivative) matrix  */
  static int gslDfx(const gsl_vector * x, void * params, gsl_matrix * J) {
    // Store parameter values
    const GslMultiFitParmsT * gslParams = ((GslMultiFitParmsT*) params);
    
    // Store current coefficients
    float a = gsl_vector_get(x, CurveParamsT::A_IDX);
    float b = gsl_vector_get(x, CurveParamsT::B_IDX);
    float c = gsl_vector_get(x, CurveParamsT::C_IDX);
    
    // Store non-changing calculations
    const float a2 = a * a;
    const float a3 = a * a2;
    
    for(size_t i = 0; i < gslParams->size(); ++i) {
      const GslMultiFitDataT & gslData = gslParams->at(i);
      
      const float oneBySigma = 1.0f / gslData.sigma;
      const float x = gslData.x;
      const float x_minus_c = x - c;
      
      gsl_matrix_set(J, i, CurveParamsT::A_IDX, -oneBySigma * b * (x_minus_c * x_minus_c) / (a3 * phi(x, a, c)));
      gsl_matrix_set(J, i, CurveParamsT::B_IDX, oneBySigma * phi(x, a, c));
      gsl_matrix_set(J, i, CurveParamsT::C_IDX, -oneBySigma * b * x_minus_c / (a2 * phi(x, a, c)));
      gsl_matrix_set(J, i, CurveParamsT::D_IDX, oneBySigma);
    }
    
    return GSL_SUCCESS;
  }
  
  /* Invokes f(x) and f'(x) */
  static int gslFdfx(const gsl_vector * x, void * params, gsl_vector * f, gsl_matrix * J) {
    gslFx(x, params, f);
    gslDfx(x, params, J);
    
    return GSL_SUCCESS;
  }
};


/**
 *
 */
template<typename T>
static inline double Lerp(T v0, T v1, T t)
{
  return (1 - t) * v0 + t * v1;
}

/**
 * Based on proposal from https://stackoverflow.com/questions/11964552/finding-quartiles
 */
template<typename T, typename RngT>
static inline T quantile(const RngT & inData, T prob)
{
  if (inData.empty()) { return 0; } // TODO: Is returning 0 ok?
  if (1 == inData.size()) { return *inData.begin(); }

  std::vector<T> data;
  std::copy(inData.begin(), inData.end(), std::back_inserter(data));
  std::sort(data.begin(), data.end());
    
  T quantile;
  
  T poi = Lerp<T>(-0.5, data.size() - 0.5, prob);
  
  size_t left = std::max(int64_t(std::floor(poi)), int64_t(0));
  size_t right = std::min(int64_t(std::ceil(poi)), int64_t(data.size() - 1));
  
  T datLeft = data.at(left);
  T datRight = data.at(right);
  
  T q = Lerp<T>(datLeft, datRight, poi - left);
  
  return q;
}


/**
 *
 */
//typedef CurveFitTmplT<HyperbolFitTraitsT> HyperbolMatcherT;
typedef HyperbolFitTraitsT::CurveParamsT VCurveParamsT;

typedef boost::geometry::model::d2::point_xy<float> DataPointT;
typedef std::vector<DataPointT> DataPointContainerT;

typedef std::pair<DataPointT,float> OutlierStatsEntryT;
typedef std::vector<OutlierStatsEntryT> OutlierStatsT;


/**********************************************************************
 * Calculation of asymptotes / slopes
 **********************************************************************/
// static inline float slope(float x, const VCurveParamsT::Type & vCurveParms) const { return -(b/a) * (x-c) + d; }
// static float left_slope(float x, const VCurveParamsT::Type & vCurveParms) const { return -slope(x, vCurveParms); }
// static float right_slope(float x, const VCurveParamsT::Type & vCurveParms) const { return slope(x, vCurveParms); }

/**
 *
 */
class FittingStatsT {
private:
  OutlierStatsT outlierStats;
  DataPointContainerT matchedDataPoints;
  VCurveParamsT::TypeT curveParms;
  float outlierBoundary;
  size_t numIterations;
  size_t numInitialDataPoints;
  size_t numMinRequiredDataPoints;
  size_t numValidDataPoints;
  bool insufficientDataPoints;
  
public:
  const OutlierStatsT & getOutlierStats() { return outlierStats; }
  void setOutlierStats(const OutlierStatsT & inOutlierStats) { outlierStats = inOutlierStats; }

  const DataPointContainerT & getMatchedDataPoints() { return matchedDataPoints; }
  void setMatchedDataPoints(const DataPointContainerT & inMatchedDataPoints) {
    matchedDataPoints = inMatchedDataPoints;
    
    // Sort data points by x coordinate
    std::sort(matchedDataPoints.begin(), matchedDataPoints.end(), [](const DataPointT & p1, const DataPointT & p2) { return p1.x() < p2.x(); });
  }
  
  const VCurveParamsT::TypeT & getCurveParms() const { return curveParms; }
  void setCurveParms(const VCurveParamsT::TypeT & inCurveParms) { curveParms = inCurveParms; }

  float getOutlierBoundary() const { return outlierBoundary; }
  void setOutlierBoundary(float inOutlierBoundary) { outlierBoundary = inOutlierBoundary; }

  size_t getNumIterations() const { return numIterations; }
  void setNumIterations(size_t inNumIterations) { numIterations = inNumIterations; }

  size_t getNumInitialDataPoints() const { return numInitialDataPoints; }
  void setNumInitialDataPoints(size_t inNumInitialDataPoints) { numInitialDataPoints = inNumInitialDataPoints; }

  size_t getNumMinRequiredDataPoints() const { return numMinRequiredDataPoints; }
  void setNumMinRequiredDataPoints(size_t inNumMinRequiredDataPoints) { numMinRequiredDataPoints = inNumMinRequiredDataPoints; }

  size_t getNumValidDataPoints() const { return numValidDataPoints; }
  void setNumValidDataPoints(size_t inNumValidDataPoints) { numValidDataPoints = inNumValidDataPoints; }

  size_t getInsufficientDataPoints() const { return insufficientDataPoints; }
  void setInsufficientDataPoints(bool inInsufficientDataPoints) { insufficientDataPoints = inInsufficientDataPoints; }
  
  
  friend std::ostream & operator<<(std::ostream & os, const FittingStatsT & stats);
};

std::ostream & operator<<(std::ostream & os, const FittingStatsT & stats) {  
  static const char * prefix = "   ";

  os << prefix << "V-Curve parameters" << std::endl
     << prefix << " > a=" << stats.curveParms[VCurveParamsT::A_IDX] << std::endl
     << prefix << " > b=" << stats.curveParms[VCurveParamsT::B_IDX] << std::endl
     << prefix << " > c=" << stats.curveParms[VCurveParamsT::C_IDX] << " (opt. position)" << std::endl
     << prefix << " > d=" << stats.curveParms[VCurveParamsT::D_IDX] << std::endl << std::endl;

  os << prefix << "Matched data points" << std::endl;
  std::for_each(stats.matchedDataPoints.begin(),
		stats.matchedDataPoints.end(),
		[&](const DataPointT & dp) -> void { os << prefix << " (x,y)=(" << dp.x() << "," << dp.y() << ")" << std::endl; }
		);
  os << std::endl;

  os << prefix << "Outliers" << std::endl;
  std::for_each(stats.outlierStats.begin(),
		stats.outlierStats.end(),
		[&](const OutlierStatsEntryT & e) -> void {
		  os << prefix << " (x,y)=(" << e.first.x() << "," << e.first.y() << "), residual=" << e.second << std::endl;
		});
  os << std::endl;
  
  os << prefix << "Number of iterations=" << stats.numIterations << std::endl
     << prefix << "Number of initial data points=" << stats.numInitialDataPoints << std::endl
     << prefix << "Minimum number of required data points=" << stats.numMinRequiredDataPoints << std::endl
     << prefix << "Number of data points within boundary=" << stats.numValidDataPoints << std::endl
     << prefix << "Outlier boundary=" << stats.outlierBoundary << std::endl
     << prefix << "Have insufficient data points=" << (stats.insufficientDataPoints ? "yes(!)" : "no") << std::endl;

  return os;  
};


/**
 *
 */
class CurveFitExceptionT : public std::exception {
private:
  std::string description;
  FittingStatsT fittingStats;
  
public:
  CurveFitExceptionT(const char * inDescription, const FittingStatsT & inFittingStats) : description(inDescription), fittingStats(inFittingStats) {};
  ~CurveFitExceptionT() throw() {};
  const char *what() const throw() { return this->description.c_str(); };
  const FittingStatsT & getFittingStats() const { return fittingStats; };
};


/**
 *
 * TODO: Improve init of dataPointsToFit by reserving space?!
 * TODO: We may pass boundary detection rule as lambda instead of factor 1.5 to fitVCurve()...
 * TODO: Evaluate return value (status) of fitGslLevenbergMarquart() call -> gsl_strerror(status)?
 * TODO: What to pass as epsAbs and epsRel?
 * TODO: Is it ok to write ss.str().c_str() since result is only temporary valid?
 */
template <typename LMFitTraitsT, typename RngT> /*typename CurveFitterT - todo: pass as lambda instead..*/
VCurveParamsT::TypeT curveFitWithOutlierDetection(const RngT & inDataPointsToFit, float inMaxAcceptedOutliersPerc = 20.0f, float inBoundaryFac = 1.5f, float inEpsAbs = 0.1f, float inEpsRel = 0.1f, FittingStatsT * outFittingStats = nullptr) {
  
  using namespace boost::adaptors;

  size_t numIterations = 0, numCurrOutliers, numValidDataPoints;
  size_t numInitialDataPoints = inDataPointsToFit.size();
  size_t numMinRequiredDataPoints = numInitialDataPoints * (1.0f - inMaxAcceptedOutliersPerc / 100.0f);
  bool insufficientDataPoints;

  // Outlier stats
  OutlierStatsT outlierStats;
  
  // Take an initial copy of the incoming data points.
  DataPointContainerT dataPointsToFit;
  boost::range::copy(inDataPointsToFit, std::back_inserter(dataPointsToFit));

  // Initialize hyperbolCurveParms - unfortunately required to calc q3 outside the loop.
  //VCurveParamsT::TypeT hyperbolCurveParms;
  VCurveParamsT::TypeT curveParms;
  
  levenbergMarquartCurveFit<LMFitTraitsT>(dataPointsToFit, & curveParms, inEpsAbs, inEpsRel);

  // Lambda-function to calculate residual.
  auto residualFunction = [&](const DataPointT & p) { return fabs(p.y() - LMFitTraitsT::fx(p.x(), curveParms)); };

  // Calculate outlier boundary - just the first time since otherwise q3 may get smaller and smaller
  // with each iteration and produce new outliers again.
  auto residuals = dataPointsToFit | transformed(residualFunction);
  auto outlierBoundary = inBoundaryFac * quantile<float>(residuals, 0.75) /*q3*/;

  // Sort criteria: Smallest residual first, if residual equal, greatest y first (because data points further
  // outside V-Curve are potentially more important).
  auto less = [&](const DataPointT & p1, const DataPointT & p2) -> bool {
    float residual1 = residualFunction(p1);
    float residual2 = residualFunction(p2);
    return ((fabs(residual1 - residual2) > std::numeric_limits<float>::epsilon()) ? residual1 < residual2 : p1.y() > p2.y());
  };
  
  
  do {    
    ++numIterations;
    
    levenbergMarquartCurveFit<LMFitTraitsT>(dataPointsToFit, & curveParms, inEpsAbs, inEpsRel);

    boost::sort(dataPointsToFit, less);
    
    auto itUp = boost::range::upper_bound(dataPointsToFit,
					  outlierBoundary,
					  [&](float rhs, const DataPointT & p) -> bool {
					    return residualFunction(p) > rhs;
					  });
    
    numValidDataPoints     = std::distance(dataPointsToFit.begin(), itUp);
    numCurrOutliers        = dataPointsToFit.size() - numValidDataPoints;
    insufficientDataPoints = (numValidDataPoints < numMinRequiredDataPoints);
    
    size_t limitIdx = (insufficientDataPoints ? numMinRequiredDataPoints : numValidDataPoints);

    // Add outliers to outlier stats
    std::transform(itUp,
		   dataPointsToFit.end(),
		   std::back_inserter(outlierStats),
		   [&](const DataPointT & p) { return std::make_pair(p, residualFunction(p)); });
    
    // "Remove" elements behind limitIdx by simply resizing the vector
    // https://stackoverflow.com/questions/21493414/keeping-the-first-n-elements-of-a-stdvector-and-removing-the-rest
    dataPointsToFit.resize(limitIdx);
    
  } while(numCurrOutliers > 0 && ! insufficientDataPoints);

  
  // Lambda to build fitting stats
  auto buildFittingStats = [&]() -> FittingStatsT {
    FittingStatsT fittingStats;
    fittingStats.setOutlierStats(outlierStats);
    fittingStats.setMatchedDataPoints(dataPointsToFit);
    fittingStats.setCurveParms(curveParms);
    fittingStats.setOutlierBoundary(outlierBoundary);
    fittingStats.setNumIterations(numIterations);
    fittingStats.setNumInitialDataPoints(numInitialDataPoints);
    fittingStats.setNumMinRequiredDataPoints(numMinRequiredDataPoints);
    fittingStats.setNumValidDataPoints(numValidDataPoints);
    fittingStats.setInsufficientDataPoints(insufficientDataPoints);
    return fittingStats;
  };

  
  // Check if fitting was finally successful
  if (insufficientDataPoints) {
    std::stringstream ss;
    ss << "Not enough data points with sufficient model accuracy. Require at least "
       << numMinRequiredDataPoints << " datapoints but only have "
       << dataPointsToFit.size() << " / " << numInitialDataPoints << "." << std::endl;
    
    throw CurveFitExceptionT(ss.str().c_str(), buildFittingStats());
  }


  if (outFittingStats != nullptr) {
    *outFittingStats = buildFittingStats();
  }
  
  return curveParms;
}


/**********************************************************************
 * Main
 **********************************************************************/
int main(int argc,  char** argv) {

  // Check arguments
  if (argc != 2) {
    std::cout << "Usage: hyperbol_fit <path/to/datafile>" << std::endl;
    return -1;
  }
  
  std::string filename(argv[1]);
  
  // Fill data container with some hyperbol shaped data
  std::list<DataPointT> dataPointsToFit;

  // Read data file
  std::ifstream infile(filename);
  float x, y;
  while (infile >> x >> y) {
    dataPointsToFit.push_back(DataPointT(x,y));
  }

  try {
    VCurveParamsT::TypeT vCurveParms;
    FittingStatsT fittingStats;

    
    // Do the fit  
    vCurveParms = curveFitWithOutlierDetection<HyperbolFitTraitsT, std::list<DataPointT> >(dataPointsToFit,
					       20.0f, // perc.
					       1.5f,  // outlier boundary factor
					       0.1f,  // inEpsAbs
					       0.1f,  // inEpsRes
					       & fittingStats);

    // Print calculated curve parameters
    std::cout << "V-Curve fit successful. Curve-parameters: " << std::endl
	      << " > a=" << vCurveParms[VCurveParamsT::A_IDX] << std::endl
	      << " > b=" << vCurveParms[VCurveParamsT::B_IDX] << std::endl
	      << " > c=" << vCurveParms[VCurveParamsT::C_IDX] << " (opt. position)" << std::endl
	      << " > d=" << vCurveParms[VCurveParamsT::D_IDX] << std::endl << std::endl;

    std::cout << "Fit statistics:" << std::endl
	      << fittingStats << std::endl << std::endl;

  } catch (CurveFitExceptionT & exc) {
    std::cerr << "Curve fit failed: " << exc.what() << std::endl
	      << exc.getFittingStats() << std::endl;
    return -1;
  }  

  return 0;
}
