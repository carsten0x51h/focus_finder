#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_POINT_PTREE_TRANSLATOR_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_POINT_PTREE_TRANSLATOR_H_

#include "point.h"
#include "validation_exception.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <boost/regex.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>

template<typename T> 
struct PointTranslatorT
{
  typedef std::string internal_type;
  typedef PointT<T>    external_type;

  boost::optional<external_type> get_value(const internal_type& str)
  {
    if (!str.empty())
      {
	using boost::algorithm::iequals;

	static boost::regex r("(-?[0-9]*\\.?[0-9]+)(,)(-?[0-9]*\\.?[0-9]+)");
	boost::smatch match;
	boost::any v;

	if (boost::regex_match(str, match, r)) {
	  v = boost::any(PointT(boost::lexical_cast<T>(match[1]), boost::lexical_cast<T>(match[3])));
	} else {
	  std::stringstream ss;
	  ss << "Invalid PointT value: '" << str << "'" << std::endl;
	  throw ValidationExceptionT(ss.str());
	}

	return boost::optional<external_type>(boost::any_cast<PointT<T> >(v));
      }
    else
      return boost::optional<external_type>(boost::none);
  }

  
  boost::optional<internal_type> put_value(const external_type& b)
  {
    std::stringstream ss;
    ss << b.x() << "," << b.y();
    
    return boost::optional<internal_type>(ss.str());
  }
};


namespace boost {
  namespace property_tree {

    template<typename Ch, typename Traits, typename Alloc> 
    struct translator_between<std::basic_string< Ch, Traits, Alloc >, PointT<float> >
    {
      typedef PointTranslatorT<float> type;
    };

    template<typename Ch, typename Traits, typename Alloc> 
    struct translator_between<std::basic_string< Ch, Traits, Alloc >, PointT<int> >
    {
      typedef PointTranslatorT<int> type;
    };

  } // namespace property_tree
}

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_POINT_PTREE_TRANSLATOR_H_*/
