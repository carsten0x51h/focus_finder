#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <tuple> // For std::tie

#include <gsl/gsl_multifit_nlin.h>

using namespace std;
 
/**********************************************************************
 * Helper classes
 **********************************************************************/
struct DataPointT {
  float x;
  float y;
  DataPointT(float inX = 0, float inY = 0) : x(inX), y(inY) {}
};
  
typedef vector<DataPointT> DataPointsT;
  
struct GslMultiFitDataT {
  float y;
  float sigma;
  DataPointT pt;
};
  
typedef vector<GslMultiFitDataT> GslMultiFitParmsT;
 
 
/**********************************************************************
 * Curve to fit to is supplied by traits.
 **********************************************************************/
template <class FitTraitsT>
class CurveFitTmplT {
public:
  typedef typename FitTraitsT::CurveParamsT CurveParamsT;
 
  /**
   * DataAccessor allows specifying how x,y data is accessed.
   * See http://en.wikipedia.org/wiki/Approximation_error for expl. of rel and abs errors.
   */
  template<typename DataAccessorT> static int  
  fitGslLevenbergMarquart(const typename DataAccessorT::TypeT & inData, typename CurveParamsT::TypeT * outResults,
				          double inEpsAbs, double inEpsRel, size_t inNumMaxIter = 500) {
    GslMultiFitParmsT gslMultiFitParms(inData.size());
      
    // Fill in the parameters
    for (typename DataAccessorT::TypeT::const_iterator it = inData.begin(); it != inData.end(); ++it) {
      size_t idx = std::distance(inData.begin(), it);
      const DataPointT & dataPoint = DataAccessorT::getDataPoint(idx, it);
      gslMultiFitParms[idx].y     = dataPoint.y;
      gslMultiFitParms[idx].sigma = 0.1f;
      gslMultiFitParms[idx].pt    = dataPoint;
    }
 
    // Fill in function info
    gsl_multifit_function_fdf f;
    f.f      = FitTraitsT::gslFx;
    f.df     = FitTraitsT::gslDfx;
    f.fdf    = FitTraitsT::gslFdfx;
    f.n      = inData.size();
    f.p      = FitTraitsT::CurveParamsT::_Count;
    f.params = & gslMultiFitParms;
    
 
    gsl_vector * guess = gsl_vector_alloc(FitTraitsT::CurveParamsT::_Count);  // Allocate the guess vector
    
    FitTraitsT::makeGuess(gslMultiFitParms, guess);  // Make initial guesses - here we just set all parameters to 1.0
    
    // Create a Levenberg-Marquardt solver with n data points and m parameters
    gsl_multifit_fdfsolver * solver = gsl_multifit_fdfsolver_alloc(gsl_multifit_fdfsolver_lmsder,
                                                                  inData.size(), FitTraitsT::CurveParamsT::_Count);
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
    for (size_t i = 0; i < FitTraitsT::CurveParamsT::_Count; ++i) {
      typename FitTraitsT::CurveParamsT::TypeE idx = static_cast<typename FitTraitsT::CurveParamsT::TypeE>(i);
      (*outResults)[idx] = gsl_vector_get(solver->x, idx);
    }
 
    // Free GSL memory
    gsl_multifit_fdfsolver_free(solver);
    gsl_vector_free(guess);
 
    return status;
  }
};


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
   * In this case we just initialize all parameters with 1.
   */
  static void makeGuess(const GslMultiFitParmsT & inData, gsl_vector * guess) {
    // Find min HFD -> good start value for c
    GslMultiFitParmsT::const_iterator minEl, maxEl;
    std::tie(minEl, maxEl) = std::minmax_element(begin(inData), end(inData),
						 [] (GslMultiFitDataT const& s1, GslMultiFitDataT const& s2)
						 {
						   return s1.pt.y < s2.pt.y;
						 });
    
    gsl_vector_set(guess, CurveParamsT::A_IDX, 1.0);
    gsl_vector_set(guess, CurveParamsT::B_IDX, 2.0);
    gsl_vector_set(guess, CurveParamsT::C_IDX, minEl->pt.x);
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
      float yi = HyperbolFitTraitsT::fx((float) gslData.pt.x, curveParams);

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
      const float x = gslData.pt.x;
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


/**********************************************************************
 * Calculation of asymptotes / slopes
 **********************************************************************/
float left_slope(float x, float a, float b, float c, float d) {
  return -(b/a) * (x-c) + d;
}
float right_slope(float x, float a, float b, float c, float d) {
  return (b/a) * (x-c) + d;
}


/**********************************************************************
 * Custom data structure + accessor
 **********************************************************************/
typedef pair<float, float> MyDataPointT;
typedef list<MyDataPointT> MyDataContainerT;
 
class MyDataAccessorT {
public:
  typedef MyDataContainerT TypeT;
  static DataPointT getDataPoint(size_t inIdx, TypeT::const_iterator inIt) {
    DataPointT dp(inIt->first /*inIdx*/, inIt->second /*y*/);
    return dp;
  }
};


/**********************************************************************
 * Main
 **********************************************************************/
int main(int argc,  char** argv) {

  // Check argument
  if (argc != 2) {
    std::cout << "Usage: hyperbol_fit <path/to/datafile>" << std::endl;
    return -1;
  }
  
  std::string filename(argv[1]);
  
  // Fill data container with some hyperbol shaped data
  MyDataContainerT dataPointsToFit;

  // Read data file
  std::ifstream infile(filename);
  MyDataPointT dp;
  while (infile >> dp.first >> dp.second) {
    dataPointsToFit.push_back(dp);
  }
  
  // Do the LM fit
  typedef CurveFitTmplT<HyperbolFitTraitsT> HyperbolMatcherT;
  typedef HyperbolMatcherT::CurveParamsT CurveParamsT;

  // Return value if fitGslLevenbergMarquart() is not evaluated here...
  CurveParamsT::TypeT hyperbolCurveParms;
  HyperbolMatcherT::fitGslLevenbergMarquart<MyDataAccessorT>(dataPointsToFit, & hyperbolCurveParms, 0.1f /*EpsAbs*/, 0.1f /*EpsRel*/);

  // Print calculated curve parameters
  std::cout << "a=" << hyperbolCurveParms[CurveParamsT::A_IDX] << std::endl
	    << "b=" << hyperbolCurveParms[CurveParamsT::B_IDX] << std::endl
	    << "c=" << hyperbolCurveParms[CurveParamsT::C_IDX] << std::endl
	    << "d=" << hyperbolCurveParms[CurveParamsT::D_IDX] << std::endl;

  return 0;
}
