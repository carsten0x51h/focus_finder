/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_PROFILE_MANAGER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_PROFILE_MANAGER_H_

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
  typedef boost::signals2::signal<void (void)> ActiveProfileChangedListenersT;
  ActiveProfileChangedListenersT mActiveProfileChangedListeners;

  typedef boost::signals2::signal<void (void)> ProfileListChangedListenersT;
  ProfileListChangedListenersT mProfileListChangedListeners;

  static const std::string PROFILE_CFG_FILENAME;
  
  // static?, as public? -> to be used by GUI...
  // TODO: std::string proposeProfileFilename(const std::string & profileName);

  /**
   * Concats root path with filename.
   */
  std::filesystem::path composeFullPath(const std::string & profileDirectoryName) const;

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
  void notifyActiveProfileChanged() { mActiveProfileChangedListeners(); }
  void notifyProfileListChanged() { mProfileListChangedListeners(); }


public:
  ProfileManagerT();
  ~ProfileManagerT();

  /**
   * Option is empty when there is no active profile,
   * otherwise a copy of the currently active profile.
   */
  std::optional<FocusFinderProfileT> getActiveProfile() const;


  std::string getActiveProfileDirectoryName() const;
  
  /**
   * If there is an active profile, returns the filename of the
   * currently active profile. Otherwise an empty string.
   */
  // -> TODO: Required? Should return path to "getActiveProfileDirectory()" + /profile.cfg 
  //std::string getActiveProfileFilename() const;


  /**
   * Return the root path to the profile directory.
   * 
   */
  static std::filesystem::path getProfilesRootDirectory();
  
  /**
   * Return the path to the currently active profile directory.
   * If there is no active profile, an empty string is returned.
   * Example: ~/.fofi/profile_1
   */
  std::string getActiveProfileDirectory() const;

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
  void activateProfile(const std::string & profileDirectoryName);


  /**
   * Removes the profile with the given name.
   * Only non-active profiles can be removed.?!?!?! But how to delete then?
   *
   * Throws if file filename supplied is empty.
   * Throws if profile file not found.
   * Throws if deletion of file fails.
   */
  void deleteProfile(const std::string & profileDirectoryName);


  /**
   * Throws if empty filename is passed
   * Throws if profile file with given name already exists.
   * Throws if storing of file fails.
   *
   * TODO: Pass a flag to overwrite an existing file?
   * TODO: Pass flag to control if new profile should directly be activated?
   */
  void addProfile(const std::string & profileDirectoryName, const FocusFinderProfileT & newProfile);


  /**
   * Update (replace) an existing profile.
   *
   * Throw if profile file not found.
   */
  void updateProfile(const std::string & profileDirectoryName, const FocusFinderProfileT & modifiedProfile);


  /**
   * Return a list of available profile (directory names).
   * In case there are no profiles, the vector is empty.
   *
   * The function looks for directories under 'getActiveProfileDirectory()'
   * for the file profile.cfg. If it is found, the containing directory is
   * added. Otherwise, the directory is ignored.
   */
  std::vector<std::string> getProfileDirectoryNames() const;


  /**
   * Notify listeners about the change of the currently active profile.
   */
  boost::signals2::connection registerActiveProfileChangedListener(const ActiveProfileChangedListenersT::slot_type & inCallBack) {
    return mActiveProfileChangedListeners.connect(inCallBack);
  }
  template <class T> void unregisterActiveProfileChangedListener(const T & inCallBack) {
    mActiveProfileChangedListeners.disconnect(inCallBack);
  }


  /**
   * Notify listeners that list of prifles has changed.
   */
  boost::signals2::connection registerProfileListChangedListener(const ProfileListChangedListenersT::slot_type & inCallBack) {
    return mProfileListChangedListeners.connect(inCallBack);
  }
  template <class T> void unregisterProfileListChangedListener(const T & inCallBack) {
    mProfileListChangedListeners.disconnect(inCallBack);
  }

};

#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_PROFILE_MANAGER_H_ */
