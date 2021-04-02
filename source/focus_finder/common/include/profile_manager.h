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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_PROFILE_MANAGER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_PROFILE_MANAGER_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_PROFILE_MANAGER_H_

#include <memory>
#include <map>
#include <vector>
#include <string>
#include <optional>

#include <boost/signals2.hpp>

#include "focus_finder_profile.h"
#include "exception.h"

class FocusFinderProfileT;

DEF_Exception(ProfileManager);

/**
 * FocusFinderProfile = Name is unique identifier.
 * It is possible that there is no active profile (initial state).
 */
class ProfileManagerT {
private:
    typedef boost::signals2::signal<void(std::optional<FocusFinderProfileT>,
                                         std::optional<FocusFinderProfileT>)> ActiveProfileChangedListenersT;
    ActiveProfileChangedListenersT mActiveProfileChangedListeners;

    typedef boost::signals2::signal<void(void)> ProfileListChangedListenersT;
    ProfileListChangedListenersT mProfileListChangedListeners;

    static const std::string PROFILE_CFG_FILENAME;

    // static?, as public? -> to be used by GUI...
    // TODO: std::string proposeProfileFilename(const std::string & profileName);

    /**
     * Concats the directory path to the given profile.
     */
    static std::filesystem::path composeFullProfileDirectory(const std::string &profileDirectoryName) ;

    /**
     * Concats root path with filename.
     */
    static std::filesystem::path composeFullProfileFilePath(const std::string &profileDirectoryName) ;

    /**
     * Concats path to light frame directory of given profile.
     */
    static std::filesystem::path composeFullLightFrameDirectory(const std::string &profileDirectoryName) ;

    /**
     * Holds the active profile (if any).
     */
    std::optional<FocusFinderProfileT> mActiveProfile;

    /**
     * Holds the directory name to the actice profile.
     */
    std::string mActiveProfileDirectoryName;

    /**
     * Holds root path where profiles are stored.
     */
    //std::string mProfileRootPath; // Location where profiles are stored


protected:
    void notifyActiveProfileChanged(std::optional<FocusFinderProfileT> oldProfile,
                                    std::optional<FocusFinderProfileT> newProfile) {
        mActiveProfileChangedListeners(oldProfile, newProfile);
    }

    void notifyProfileListChanged() { mProfileListChangedListeners(); }


public:
    ProfileManagerT();

    ~ProfileManagerT();

    /**
     * Option is empty when there is no active profile,
     * otherwise a copy of the currently active profile.
     */
    std::optional<FocusFinderProfileT> getActiveProfile() const;


    /**
     * If there is an active profile, returns the filename of the
     * currently active profile. Otherwise an empty string.
     */
    // -> TODO: Required? Should return path to "getActiveProfileDirectory()" + /profile.cfg
    //std::string getActiveProfileFilename() const;


    /**
     * Return the root path to the profile directory.
     * Example: ~/.fofi/profiles
     */
    static std::filesystem::path getProfilesRootDirectory();

    /**
     * Return the directory name of the currently active profile.
     * If there is no active profile, an empty string is returned.
     * Example: profile_1
     */
    std::string getActiveProfileDirectoryName() const;

    /**
     * Return the path to the currently active profile directory.
     * If there is no active profile, an empty string is returned.
     *
     * Example: ~/.fofi/profiles/profile_1
     */
    std::optional<std::filesystem::path> getActiveProfileDirectory() const;

    /**
     * Optionally returns the active profile lightframe directory where the
     * star images of the calibration are stored. If there is no active
     * profile, the empty string is returned.
     *
     * Example: ~/.fofi/profile_1/calibration/light_frames
     */
    std::optional<std::filesystem::path> getActiveProfileLightFrameDirectory() const;

    /**
     * Remove active profile so that no profile is active any longer.
     */
    void clearActiveProfile();


    /**
     * Sets the profile with the given name as active profile
     * and notifies about the change.
     *
     * If active profile already is set, just return.
     * Throws if profile not found.
     */
    void activateProfile(const std::string &profileDirectoryName);


    /**
     * Removes the profile with the given name.
     * Only non-active profiles can be removed.?!?!?! But how to delete then?
     *
     * Throws if file filename supplied is empty.
     * Throws if profile file not found.
     * Throws if deletion of file fails.
     */
    void deleteProfile(const std::string &profileDirectoryName);


    /**
     * Throws if empty filename is passed
     * Throws if profile file with given name already exists.
     * Throws if storing of file fails.
     *
     * TODO: Pass a flag to overwrite an existing file?
     * TODO: Pass flag to control if new profile should directly be activated?
     */
    void addProfile(const std::string &profileDirectoryName, const FocusFinderProfileT &newProfile);


    /**
     * Update (replace) an existing profile.
     *
     * Throw if profile file not found.
     */
    void updateProfile(const std::string &profileDirectoryName, const FocusFinderProfileT &modifiedProfile);


    /**
     * Return a list of available profile (directory names).
     * In case there are no profiles, the vector is empty.
     *
     * The function looks for directories under 'getActiveProfileDirectory()'
     * for the file profile.cfg. If it is found, the containing directory is
     * added. Otherwise, the directory is ignored.
     *
     * Example:
     *
     * profile_1
     * profile_2
     * profile_3
     */
    static std::vector<std::string> getProfileDirectoryNames() ;


    /**
     * Notify listeners about the change of the currently active profile.
     */
    boost::signals2::connection
    registerActiveProfileChangedListener(const ActiveProfileChangedListenersT::slot_type &inCallBack) {
        return mActiveProfileChangedListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterActiveProfileChangedListener(const T &inCallBack) {
        mActiveProfileChangedListeners.disconnect(inCallBack);
    }


    /**
     * Notify listeners that list of prifles has changed.
     */
    boost::signals2::connection
    registerProfileListChangedListener(const ProfileListChangedListenersT::slot_type &inCallBack) {
        return mProfileListChangedListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterProfileListChangedListener(const T &inCallBack) {
        mProfileListChangedListeners.disconnect(inCallBack);
    }

};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_PROFILE_MANAGER_H_ */
