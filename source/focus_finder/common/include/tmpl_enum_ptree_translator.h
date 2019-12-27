#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TMPL_ENUM_PTREE_TRANSLATOR_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TMPL_ENUM_PTREE_TRANSLATOR_H_

#include "validation_exception.h"

#include <boost/property_tree/ptree.hpp>

template<typename T> 
struct TmplEnumTranslatorT
{
  typedef std::string internal_type;
  typedef typename T::TypeE external_type;

  boost::optional<external_type> get_value(const internal_type& str) {
    if (!str.empty()) {
      typename T::TypeE t = T::asType(str.c_str());
      
      if (t == T::_Count) {
	std::stringstream ss;
	ss << "Invalid enum value: '" << str << "'" << std::endl;
	throw ValidationExceptionT(ss.str());
      }
      
      return boost::optional<external_type>(t);
    }
    else {
      return boost::optional<external_type>(boost::none);
    }
  }
  
  boost::optional<internal_type> put_value(const external_type& b) {
    return boost::optional<internal_type>(T::asStr(b));
  }
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TMPL_ENUM_PTREE_TRANSLATOR_H_*/
