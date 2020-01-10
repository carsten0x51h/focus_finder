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


// TODO: -> one validator .h files for each type? What about int/float? -> both into same file....
// TODO: What about << operator? Can we get the tuple<< work and use this onea always? Can we configure it so that entries are sep. by , ?

////////////////////////////////////////////////////////
// INI Parsing of custom types - for boost property ptree...
////////////////////////////////////////////////////////
//#include <boost/regex.hpp>
//
////BinningT
//
//
//static regex r("([0-9]+)(x|,)([0-9]+)");
//smatch match;
//if (regex_match(s, match, r)) {
//  v = any(BinningT(lexical_cast<int>(match[1]), lexical_cast<int>(match[3])));
//} else {
//  throw validation_error(validation_error::invalid_option_value);
//}
//
//
////FrameT float
//void validate(boost::any & v, const vector<string> & values, FrameT<float> * target_type, int) {
//  using namespace boost::program_options;
//
//  static regex r("([0-9]+)(x|,)([0-9]+)(x|,)([0-9]+)(x|,)([0-9]+)");
//  validators::check_first_occurrence(v);
//  const string & s = validators::get_single_string(values);
//
//  smatch match;
//  if (regex_match(s, match, r)) {
//    v = any(FrameT<float>(lexical_cast<int>(match[1]), lexical_cast<int>(match[3]), lexical_cast<int>(match[5]), lexical_cast<int>(match[7])));
//  } else {
//    throw validation_error(validation_error::invalid_option_value);
//  }
//
//  // FrameT int
//  static regex r("([0-9]+)(x|,)([0-9]+)(x|,)([0-9]+)(x|,)([0-9]+)");
//  validators::check_first_occurrence(v);
//  const string & s = validators::get_single_string(values);
//
//  smatch match;
//  if (regex_match(s, match, r)) {
//    v = any(FrameT<int>(lexical_cast<int>(match[1]), lexical_cast<int>(match[3]), lexical_cast<int>(match[5]), lexical_cast<int>(match[7])));
//  } else {
//    throw validation_error(validation_error::invalid_option_value);
//  }
//
//  // Point float
//  // Note: Also used for DimensionT since same tupple type...
//  void validate(boost::any & v, const vector<string> & values, PointT<float> * target_type, int) {
//    using namespace boost::program_options;
//
//    static regex r("([0-9]*\\.?[0-9]+)(x|,)([0-9]*\\.?[0-9]+)");
//    validators::check_first_occurrence(v);
//    const string & s = validators::get_single_string(values);
//
//    smatch match;
//    if (regex_match(s, match, r)) {
//
//      // TODO/FIXME: The any cast fails... somehow... any -> Point
//
//      v = any(PointT<float>(lexical_cast<float>(match[1]), lexical_cast<float>(match[3])));
//    } else {
//      throw validation_error(validation_error::invalid_option_value);
//    }
//
//    // Point int
//    void validate(boost::any & v, const vector<string> & values, PointT<int> * target_type, int) {
//      using namespace boost::program_options;
//
//      static regex r("([0-9]*\\.?[0-9]+)(x|,)([0-9]*\\.?[0-9]+)");
//      validators::check_first_occurrence(v);
//      const string & s = validators::get_single_string(values);
//
//      smatch match;
//      if (regex_match(s, match, r)) {
//        v = any(PointT<int>(lexical_cast<int>(match[1]), lexical_cast<int>(match[3])));
//      } else {
//        throw validation_error(validation_error::invalid_option_value);
//      }
//
//      // Hostname, port
//      void validate(boost::any & v, const vector<string> & values, HostnameAndPortT * target_type, int) {
//        using namespace boost::program_options;
//
//        static regex r("(\\w+)(:)([0-9]+)");
//        validators::check_first_occurrence(v);
//        const string & s = validators::get_single_string(values);
//
//        smatch match;
//        if (regex_match(s, match, r)) {
//          v = any(HostnameAndPortT(lexical_cast<string>(match[1]), lexical_cast<int>(match[3])));
//        } else {
//          throw validation_error(validation_error::invalid_option_value);
//        }
