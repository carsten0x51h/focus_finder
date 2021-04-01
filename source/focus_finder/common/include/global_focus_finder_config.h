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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_GLOBAL_FOCUS_FINDER_CONFIG_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_GLOBAL_FOCUS_FINDER_CONFIG_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_GLOBAL_FOCUS_FINDER_CONFIG_H_

#include <string>

#include "timestamp.h"
#include "timestamp_ptree_translator.h"

#include "exception.h"
#include "device_manager_type.h"
#include "device_manager_type_ptree_translator.h"

// Required ?  #include "device_manager_factory.h"


DEF_Exception(GlobalFocusFinderConfig);

class GlobalFocusFinderConfigT {
public:
    GlobalFocusFinderConfigT();

    ~GlobalFocusFinderConfigT();

    [[nodiscard]] const TimestampT &getLastChanged() const;

    void setLastChanged(const TimestampT &lastChanged);


    /**
     * Last active focus finder profile
     */
    [[nodiscard]] std::string getLastActiveFocusFinderProfileName() const;

    void setLastActiveFocusFinderProfileName(const std::string &lastActiveFocusFinderProfileName);


    /**
     * Device manager type name.
     *
     * NOTE: Currently only DUMMY and INDI is supported.
     *       ASCOM may be supported in the future well...
     */
    [[nodiscard]] DeviceManagerTypeT::TypeE getDeviceManagerType() const;

    void setDeviceManagerType(DeviceManagerTypeT::TypeE deviceManagerType);

    /**
     * Device manager specific config.
     *
     * @return
     */
    [[nodiscard]] boost::property_tree::ptree getDeviceManagerConfig() const;
    void setDeviceManagerConfig(const boost::property_tree::ptree & deviceManagerConfig);



    /**
     * Throws if not found.
     */
    static GlobalFocusFinderConfigT load(const std::string &fullGlobalFocusFinderConfigPath);

    /**
     * Throws if save fails.
     */
    static void save(const std::string &fullGlobalFocusFinderConfigPath,
                     const GlobalFocusFinderConfigT &profile);

    std::ostream &print(std::ostream &os, size_t ident = 0) const;


    static const GlobalFocusFinderConfigT &defaults();

private:
    static const GlobalFocusFinderConfigT sDefaultGlobalFocusFinderConfig;


    TimestampT mLastChanged;
    std::string mLastActiveFocusFinderProfileName;
    DeviceManagerTypeT::TypeE mDeviceManagerType;
    boost::property_tree::ptree mDeviceManagerConfig;
};

// TODO: Remove... member access no longer required since print() is now used..
std::ostream &operator<<(std::ostream &os, const GlobalFocusFinderConfigT &globalFocusFinderConfig);

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_GLOBAL_FOCUS_FINDER_CONFIG_H_ */


