#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FITTING_CURVE_TYPE_PTREE_TRANSLATOR_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FITTING_CURVE_TYPE_PTREE_TRANSLATOR_H_

#include "fitting_curve_type.h"
#include "tmpl_enum_ptree_translator.h"

#include <boost/property_tree/ptree.hpp>

/*  Specialize translator_between so that it uses our custom translator for
    bool value types. Specialization must be in boost::property_tree
    namespace. */
namespace boost {
  namespace property_tree {
    template<typename Ch, typename Traits, typename Alloc> 
    struct translator_between<std::basic_string< Ch, Traits, Alloc >, typename FittingCurveTypeT::TypeE>
    {
      typedef TmplEnumTranslatorT<FittingCurveTypeT> type;
    };
  } // namespace property_tree
}

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FITTING_CURVE_TYPE_PTREE_TRANSLATOR_H_*/
