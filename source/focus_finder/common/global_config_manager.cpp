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

#include <filesystem>

#include "include/logging.h"
#include "include/global_config_manager.h"
#include "include/global_focus_finder_config.h"
#include "include/home_directory_finder.h"


const std::string GlobalConfigManagerT::GLOBAL_CFG_FILENAME = "fofi_global.cfg";


std::filesystem::path GlobalConfigManagerT::getGlobalConfigRootDirectory() {
  std::filesystem::path globalConfigRootPath = HomeDirectoryFinderT::getHomeDir();

  globalConfigRootPath /= ".fofi";
  
  return globalConfigRootPath;
}


std::filesystem::path GlobalConfigManagerT::composeFullGlobalConfigFilePath() {

  std::filesystem::path fullPath = GlobalConfigManagerT::getGlobalConfigRootDirectory();

  fullPath /= GLOBAL_CFG_FILENAME;
  
  return fullPath;
}

GlobalConfigManagerT::GlobalConfigManagerT()
{
  // Create ".fofi" in home directory if it does not exist.
  // TODO: Only create if it does not exist...
  // TODO: Error handling...
  std::filesystem::create_directories(GlobalConfigManagerT::getGlobalConfigRootDirectory());

  // Try to load the config
  try {
    std::string fullPathToGlobalConfig = composeFullGlobalConfigFilePath().string();

    // TODO: Handle case when config does not exist...?! -> Create empty global config...?!
    mGlobalFocusFinderConfig = GlobalFocusFinderConfigT::load(fullPathToGlobalConfig);
    
    LOG(debug) << "Loaded global FoFi config: " << fullPathToGlobalConfig << std::endl;
    
    
  } catch (GlobalFocusFinderConfigExceptionT & exc) {
    throw GlobalConfigManagerExceptionT(exc.what());
  }
    
}

GlobalFocusFinderConfigT GlobalConfigManagerT::getConfig() const {
  return mGlobalFocusFinderConfig;
}

void GlobalConfigManagerT::setConfig(const GlobalFocusFinderConfigT & modifiedGlobalConfig) {

  std::string fullPathToGlobalConfig = composeFullGlobalConfigFilePath().string();
  
  // Store to file
  try {
    GlobalFocusFinderConfigT::save(fullPathToGlobalConfig, modifiedGlobalConfig);

    // If string was successful, set the new global config
    mGlobalFocusFinderConfig = modifiedGlobalConfig;
    
    // Notify all listeners about global config change...
    // Add if required...
    //notifyGlobalConfigChanged();

  } catch (GlobalFocusFinderConfigExceptionT & exc) {
    throw GlobalConfigManagerExceptionT(exc.what());
  }
}
