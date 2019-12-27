#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TIMESTAMP_PTREE_TRANSLATOR_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TIMESTAMP_PTREE_TRANSLATOR_H_

#include "timestamp.h"
#include "validation_exception.h"

#include <boost/property_tree/ptree.hpp>

struct TimestampTranslatorT
{
  typedef std::string internal_type;
  typedef TimestampT external_type;

  boost::optional<external_type> get_value(const internal_type& str) {
    if (!str.empty()) {
      
      try {
	long tsLong = std::stol(str);

	std::chrono::milliseconds durationInMs{ tsLong };
	TimestampT ts(durationInMs);

	return boost::optional<external_type>(ts);
      }
      catch (std::invalid_argument & exc) {
      	std::stringstream ss;
      	ss << "Invalid TimestampT value: '" << str << "'. Details: " << exc.what() << std::endl;
      	throw ValidationExceptionT(ss.str());
      }
      catch (std::out_of_range & exc) {
      	std::stringstream ss;
      	ss << "Value '" << str << "'"" is out of range for TimestampT. Details: " << exc.what() << std::endl;
      	throw ValidationExceptionT(ss.str());
      }
    }
    else {
      return boost::optional<external_type>(boost::none);
    }
  }
  
  boost::optional<internal_type> put_value(const external_type& timestamp) {
    std::stringstream ss;
    ss << std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()).count();
   
    return boost::optional<internal_type>(ss.str());
  }
};

/*  Specialize translator_between so that it uses our custom translator for
    bool value types. Specialization must be in boost::property_tree
    namespace. */
namespace boost {
  namespace property_tree {
    template<typename Ch, typename Traits, typename Alloc> 
    struct translator_between<std::basic_string< Ch, Traits, Alloc >, TimestampT>
    {
      typedef TimestampTranslatorT type;
    };
  } // namespace property_tree
}

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TIMESTAMP_PTREE_TRANSLATOR_H_*/
