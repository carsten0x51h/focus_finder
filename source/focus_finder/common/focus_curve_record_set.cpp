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

#include <tuple>

#include "include/focus_curve_record_set.h"
#include "include/focus_measure_type.h"

#include "include/focus_measure_type_ptree_translator.h"


FocusCurveRecordSetT::FocusCurveRecordSetT(FocusMeasureTypeT::TypeE focusMeasureType, float focusMeasureLimit) : mFocusMeasureType(focusMeasureType), mFocusMeasureLimit(focusMeasureLimit) {
}

float FocusCurveRecordSetT::getFocusMeasureLimit() const {
  return mFocusMeasureLimit;
}

FocusMeasureTypeT::TypeE FocusCurveRecordSetT::getFocusMeasureType() const {
  return mFocusMeasureType;
}

/**
 * Find min and max position in all records of this set.
 */
std::pair<int, int> FocusCurveRecordSetT::minmaxFocusPos() const {

  const_iterator itMin;
  const_iterator itMax;
  
  std::tie(itMin, itMax) = std::minmax_element(std::begin(*this),
					       std::end(*this),
					       [] (std::shared_ptr<const FocusCurveRecordT> focusCurveRecord1, std::shared_ptr<const FocusCurveRecordT> focusCurveRecord2)
					       {
						 return focusCurveRecord1->getCurrentAbsoluteFocusPos() < focusCurveRecord2->getCurrentAbsoluteFocusPos();
					       });
  
  return std::make_pair<int, int>((**itMin).getCurrentAbsoluteFocusPos(), (**itMax).getCurrentAbsoluteFocusPos());
}


void FocusCurveRecordSetT::save(boost::property_tree::ptree & focusCurvesPt, std::shared_ptr<FocusCurveRecordSetT> focusCurveRecordSet) {
  LOG(debug) << "FocusCurveRecordSetT::save... Storing " << focusCurveRecordSet->size() << " records..." << std::endl;

  boost::property_tree::ptree recordSetPt;

  for (auto & focusCurveRecord : *focusCurveRecordSet) {
    FocusCurveRecordT::save(recordSetPt, *focusCurveRecord);    
  }
  focusCurvesPt.add_child("record_set", recordSetPt);

  // <record_set creation_timestamp="1574134119">
  // TODO: focusCurvesPt.put("record_set.<xmlattr>.creation_timestamp", focusCurveRecordSet);

}


std::shared_ptr<FocusCurveRecordSetT> FocusCurveRecordSetT::load(const boost::property_tree::ptree & pt) {

  //get<FocusMeasureTypeT>()
  FocusMeasureTypeT::TypeE focusMeasureType = FocusMeasureTypeT::HFD; // TODO: LOAD
  float focusMeasureLimit = 18.0F; // TODO: Load
  // TODO: Load creation_timestamp....
  
  auto focusCurveRecordSet = std::make_shared<FocusCurveRecordSetT>(focusMeasureType, focusMeasureLimit);
  
  LOG(debug) << "FocusCurveRecordSetT::load..." << std::endl;

  // See https://stackoverflow.com/questions/8121042/how-to-iterate-over-childnodes-in-boostpropertytree-if-these-have-childs-itse
  for (auto & focusCurveRecordNode : pt) {

    if (focusCurveRecordNode.first == "curve_record") {
      LOG(debug) << "focusCurveRecordNode.first:  " << focusCurveRecordNode.first << std::endl;
	       // << " - abs_focus_pos: " << focusCurveRecordNode.second.get<int>("<xmlattr>.abs_focus_pos", 0)
	       // << std::endl;
      // NOTE: Other than the name suggests, focusCurveRecordSets is a std::vector...
      focusCurveRecordSet->push_back(FocusCurveRecordT::load(focusCurveRecordNode.second));
    }

  }

  return focusCurveRecordSet;
}


std::ostream & operator<<(std::ostream & os, const FocusCurveRecordSetT & focusCurveRecordSet) {
	return focusCurveRecordSet.print(os);
}

std::ostream &
FocusCurveRecordSetT::print(std::ostream & os, size_t indent) const {
  std::string prefix = std::string(indent, ' ');
  
  os << prefix << "--- FocusCurveRecordSet (#" << this->size() << " data points) ---" << std::endl << std::endl;
  
  for (FocusCurveRecordSetT::const_iterator it = this->begin(); it != this->end(); ++it) {
    const FocusCurveRecordT & fcr = **it;
    fcr.print(os, indent + 4);
    os << std::endl;
  }

  return os;
}
