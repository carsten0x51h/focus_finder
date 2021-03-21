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

#include "include/global_focus_finder_config.h"
#include "include/logging.h"

#include "include/timestamp.h"
#include "include/timestamp_ptree_translator.h"

#include "include/device_manager_type.h"
#include "include/device_manager_type_ptree_translator.h"

#include <chrono>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace std::chrono_literals;

const GlobalFocusFinderConfigT GlobalFocusFinderConfigT::sDefaultGlobalFocusFinderConfig;


GlobalFocusFinderConfigT::GlobalFocusFinderConfigT() :
        mLastChanged(std::chrono::high_resolution_clock::now()),
        mLastActiveFocusFinderProfileName(),
        mDeviceManagerType(DeviceManagerTypeT::INDI) {
    LOG(debug)
        << "GlobalFocusFinderConfigT::GlobalFocusFinderConfigT()..." << std::endl;
}

GlobalFocusFinderConfigT::~GlobalFocusFinderConfigT() {
    LOG(debug)
        << "GlobalFocusFinderConfigT::~GlobalFocusFinderConfigT()..." << std::endl;
}


const TimestampT &GlobalFocusFinderConfigT::getLastChanged() const {
    return mLastChanged;
}

void GlobalFocusFinderConfigT::setLastChanged(const TimestampT &lastChanged) {
    mLastChanged = lastChanged;
}


std::string GlobalFocusFinderConfigT::getLastActiveFocusFinderProfileName() const {
    return mLastActiveFocusFinderProfileName;
}

void
GlobalFocusFinderConfigT::setLastActiveFocusFinderProfileName(const std::string &lastActiveFocusFinderProfileName) {
    mLastActiveFocusFinderProfileName = lastActiveFocusFinderProfileName;
}


DeviceManagerTypeT::TypeE GlobalFocusFinderConfigT::getDeviceManagerType() const {
    return mDeviceManagerType;
}

void GlobalFocusFinderConfigT::setDeviceManagerType(DeviceManagerTypeT::TypeE deviceManagerType) {
    mDeviceManagerType = deviceManagerType;
}


GlobalFocusFinderConfigT GlobalFocusFinderConfigT::load(const std::string &fullGlobalFocusFinderConfigPath) {

    GlobalFocusFinderConfigT globalConfig;

    try {
        boost::property_tree::ptree pt;
        boost::property_tree::xml_parser::read_xml(fullGlobalFocusFinderConfigPath, pt);

        globalConfig.setLastChanged(pt.get("global_fofi_cfg.<xmlattr>.last_changed", defaults().getLastChanged()));
        globalConfig.setLastActiveFocusFinderProfileName(pt.get("global_fofi_cfg.last_active_focus_finder_profile_name",
                                                                defaults().getLastActiveFocusFinderProfileName()));
        globalConfig.setDeviceManagerType(
                pt.get("global_fofi_cfg.device_manager_type", defaults().getDeviceManagerType()));

        //
        // Add further fields which should need to be loaded here...
        //

    } catch (boost::property_tree::xml_parser_error &exc) {
        throw GlobalFocusFinderConfigExceptionT(exc.what());
    }

    return globalConfig;
}


void GlobalFocusFinderConfigT::save(const std::string &fullGlobalFocusFinderConfigPath,
                                    const GlobalFocusFinderConfigT &globalConfig) {
    boost::property_tree::ptree pt;

    try {

        pt.put<TimestampT>("global_fofi_cfg.<xmlattr>.last_changed", globalConfig.getLastChanged());
        pt.put<std::string>("global_fofi_cfg.last_active_focus_finder_profile_name",
                            globalConfig.getLastActiveFocusFinderProfileName());
        pt.put<DeviceManagerTypeT::TypeE>("global_fofi_cfg.device_manager_type", globalConfig.getDeviceManagerType());

        //
        // Add further fiels which should need to be stored here...
        //

        write_xml(fullGlobalFocusFinderConfigPath, pt);

    } catch (boost::property_tree::xml_parser_error &exc) {
        throw GlobalFocusFinderConfigExceptionT(exc.what());
    }
}


std::ostream &GlobalFocusFinderConfigT::print(std::ostream &os, size_t indent) const {

    std::string prefix = std::string(indent, ' ');

    os << prefix << "---Global Focus Finder Config---" << std::endl
       << prefix << "Last changed: " << mLastActiveFocusFinderProfileName << std::endl
       << prefix << "Device manager type: " << DeviceManagerTypeT::asStr(mDeviceManagerType) << std::endl;

    return os;
}


const GlobalFocusFinderConfigT &GlobalFocusFinderConfigT::defaults() {
    return sDefaultGlobalFocusFinderConfig;
}

std::ostream &operator<<(std::ostream &os, const GlobalFocusFinderConfigT &globalFocusFinderConfig) {
    return globalFocusFinderConfig.print(os);
}
