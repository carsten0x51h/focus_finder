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

#include <memory>
#include <algorithm>
#include <vector>
#include <filesystem>

#include <boost/algorithm/string/predicate.hpp>

#include "include/logging.h"
#include "include/profile_manager.h"
#include "include/focus_finder_profile.h"
#include "include/home_directory_finder.h"


const std::string ProfileManagerT::PROFILE_CFG_FILENAME = "profile.cfg";


std::filesystem::path ProfileManagerT::composeFullProfileDirectory(const std::string &profileDirectoryName) const {

    std::filesystem::path fullPath = ProfileManagerT::getProfilesRootDirectory();

    fullPath /= profileDirectoryName;

    return fullPath;
}


std::filesystem::path ProfileManagerT::composeFullProfileFilePath(
        const std::string &profileDirectoryName) const {

    std::filesystem::path fullPath = ProfileManagerT::composeFullProfileDirectory(profileDirectoryName);

    fullPath /= PROFILE_CFG_FILENAME;

    return fullPath;
}

std::filesystem::path ProfileManagerT::composeFullLightFrameDirectory(const std::string &profileDirectoryName) const {

    std::filesystem::path fullPath = ProfileManagerT::composeFullProfileDirectory(profileDirectoryName);

    fullPath /= "calibration";
    fullPath /= "light_frames";

    return fullPath;
}


ProfileManagerT::ProfileManagerT() :
        mActiveProfile(std::nullopt) {
    // Create ".fofi" in home directory if it does not exist.
    // TODO: Only create if it does not exist...
    // TODO: Error handling...
    std::filesystem::create_directories(ProfileManagerT::getProfilesRootDirectory());
}

ProfileManagerT::~ProfileManagerT() {
}

std::optional<FocusFinderProfileT> ProfileManagerT::getActiveProfile() const {
    return mActiveProfile;
}

std::string ProfileManagerT::getActiveProfileDirectoryName() const {
    return mActiveProfileDirectoryName;
}

void ProfileManagerT::clearActiveProfile() {
    std::optional<FocusFinderProfileT> oldProfile = mActiveProfile;

    mActiveProfile.reset();
    mActiveProfileDirectoryName = ""; // TODO: nullptr? or null?

    // Notify all listeners about profile change
    // TODO: Only notify if something has changed?
    notifyActiveProfileChanged(oldProfile, mActiveProfile);
}

void ProfileManagerT::activateProfile(const std::string &profileDirectoryName) {
    try {
        std::string fullPathToProfileFile = composeFullProfileFilePath(profileDirectoryName).string();
        std::filesystem::path fullLightFrameDirectory = composeFullLightFrameDirectory(profileDirectoryName);

        std::optional<FocusFinderProfileT> oldProfile = mActiveProfile;

        mActiveProfile.emplace(
                FocusFinderProfileT::load(fullPathToProfileFile, fullLightFrameDirectory)
        );

        LOG(debug) << "Active profile: " << std::endl;

        if (mActiveProfile.has_value()) {
            LOG(debug) << mActiveProfile.value() << std::endl;
        } else {
            LOG(debug) << "<not set>" << std::endl;
        }

        mActiveProfileDirectoryName = profileDirectoryName;

        // Notify all listeners about profile change
        notifyActiveProfileChanged(oldProfile, mActiveProfile);

    } catch (FocusFinderProfileExceptionT &exc) {
        throw ProfileManagerExceptionT(exc.what());
    }
}

void ProfileManagerT::deleteProfile(const std::string &profileDirectoryName) {
    if (profileDirectoryName.empty()) {
        throw ProfileManagerExceptionT("No profile directory name specified.");
    }

    std::error_code ec;
    std::string fullPathToProfileFile = composeFullProfileFilePath(profileDirectoryName).string();

    // Check if file exists
    bool exists = std::filesystem::exists(fullPathToProfileFile, ec);

    if (ec) {
        // Error even checking if file is there...
        throw ProfileManagerExceptionT(ec.message());
    }

    if (!exists) {
        throw ProfileManagerExceptionT("Profile file not found.");
    }

    // Delete profile from filesystem
    std::filesystem::remove(fullPathToProfileFile, ec);

    if (ec) {
        // Error deleting file...
        throw ProfileManagerExceptionT(ec.message());
    }

    // Notify that profile list has changed
    notifyProfileListChanged();

    // If the deleted one is the currently active profile, clear the active profile.
    if (mActiveProfileDirectoryName == profileDirectoryName) {
        LOG(debug)
            << "ProfileManagerT::deleteProfile... mActiveProfileDirectoryName = "
            << mActiveProfileDirectoryName << " == profileDirectoryName = " << profileDirectoryName
            << " -> clearing active profile..." << std::endl;

        clearActiveProfile();
    }
}

void ProfileManagerT::addProfile(const std::string &profileDirectoryName,
                                 const FocusFinderProfileT &newProfile) {
    if (profileDirectoryName.empty()) {
        throw ProfileManagerExceptionT("No profile directory name specified.");
    }

    // Check if file is there already (how to handle?)
    std::error_code ec;
    std::string fullPathToProfileFile = composeFullProfileFilePath(profileDirectoryName).string();

    // Check if file exists
    bool exists = std::filesystem::exists(fullPathToProfileFile, ec);

    if (ec) {
        // Error even checking if file is there...
        throw ProfileManagerExceptionT(ec.message());
    }

    if (exists) {
        throw ProfileManagerExceptionT("Profile file already exists.");
    }


    std::filesystem::path fullLightFrameDirectory = composeFullLightFrameDirectory(profileDirectoryName);


    // TODO: Create light_frame directory if not yet there

    // Store to file
    try {
        FocusFinderProfileT::save(fullPathToProfileFile, fullLightFrameDirectory, newProfile);
    } catch (FocusFinderProfileExceptionT &exc) {
        throw ProfileManagerExceptionT(exc.what());
    }

    // Notify that profile list has changed
    notifyProfileListChanged();
}

void ProfileManagerT::updateProfile(const std::string &profileDirectoryName,
                                    const FocusFinderProfileT &modifiedProfile) {
    if (profileDirectoryName.empty()) {
        throw ProfileManagerExceptionT("No profile directory name specified.");
    }

    std::string fullPathToProfileFile = composeFullProfileFilePath(profileDirectoryName).string();
    std::filesystem::path fullLightFrameDirectory = composeFullLightFrameDirectory(profileDirectoryName);

    // Store to file
    try {
        FocusFinderProfileT::save(fullPathToProfileFile, fullLightFrameDirectory, modifiedProfile);
    } catch (FocusFinderProfileExceptionT &exc) {
        throw ProfileManagerExceptionT(exc.what());
    }

    // In case the updated profile is currently the active one, overwrite the active one with the modified one and notify listeners.
    if (mActiveProfileDirectoryName == profileDirectoryName) {
        std::optional<FocusFinderProfileT> oldProfile = mActiveProfile;

        // Set new active profile
        mActiveProfile.emplace(modifiedProfile);
        mActiveProfileDirectoryName = profileDirectoryName;

        // Notify all listeners about profile change
        notifyActiveProfileChanged(oldProfile, mActiveProfile);
    }
}

std::optional<std::filesystem::path> ProfileManagerT::getActiveProfileDirectory() const {
    return (!mActiveProfileDirectoryName.empty() ? std::optional<std::filesystem::path>{
            ProfileManagerT::composeFullProfileDirectory(mActiveProfileDirectoryName)} : std::nullopt);
}

std::optional<std::filesystem::path> ProfileManagerT::getActiveProfileLightFrameDirectory() const {
    return (!mActiveProfileDirectoryName.empty() ? std::optional<std::filesystem::path>{
            ProfileManagerT::composeFullLightFrameDirectory(mActiveProfileDirectoryName)} : std::nullopt);
}

std::filesystem::path ProfileManagerT::getProfilesRootDirectory() {
    std::filesystem::path profileRootPath = HomeDirectoryFinderT::getHomeDir();

    profileRootPath /= ".fofi";
    profileRootPath /= "profiles";

    return profileRootPath;
}


std::vector<std::string> ProfileManagerT::getProfileDirectoryNames() const {
    std::vector<std::string> profileDirectoryNames;
    std::error_code ec;

    for (auto &p : std::filesystem::directory_iterator(ProfileManagerT::getProfilesRootDirectory())) {

        auto fullProfileFilePath = p.path();
        fullProfileFilePath /= PROFILE_CFG_FILENAME;

        bool isDirectory = std::filesystem::is_directory(p.status());
        bool profileCfgFilePathExists = std::filesystem::exists(fullProfileFilePath, ec);
        bool profileCfgFileIsRegularFile = std::filesystem::is_regular_file(fullProfileFilePath, ec);

        if (isDirectory && profileCfgFilePathExists && profileCfgFileIsRegularFile) {
            profileDirectoryNames.push_back(p.path().filename());
        }

    }
    return profileDirectoryNames;
}
