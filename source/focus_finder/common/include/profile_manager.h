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


  // static?, as public? -> to be used by GUI...
  // TODO: std::string proposeProfileFilename(const std::string & profileName);

  /**
   * Concats root path with filename.
   */
  std::string composeFullPath(const std::string & profileFilename) const;

  /**
   * Holds the active profile (if any).
   */
  std::optional<FocusFinderProfileT> mActiveProfile;

  /**
   * Holds the filename to the actice profile.
   */
  std::string mActiveProfileFilename;

  /**
   * Holds root path where profiles are stored.
   */
  std::string mProfileRootPath; // Location where profiles are stored


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


  /**
   * If there is an active profile, returns the filename of the
   * currently active profile. Otherwise an empty string.
   */
  std::string getActiveProfileFilename() const;


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
  void activateProfile(const std::string & profileFilename);


  /**
   * Removes the profile with the given name.
   * Only non-active profiles can be removed.?!?!?! But how to delete then?
   *
   * Throws if file filename supplied is empty.
   * Throws if profile file not found.
   * Throws if deletion of file fails.
   */
  void deleteProfile(const std::string & profileFilename);


  /**
   * Throws if empty filename is passed
   * Throws if profile file with given name already exists.
   * Throws if storing of file fails.
   *
   * TODO: Pass a flag to overwrite an existing file?
   * TODO: Pass flag to control if new profile should directly be activated?
   */
  void addProfile(const std::string & profileFilename, const FocusFinderProfileT & newProfile);


  /**
   * Update (replace) an existing profile.
   *
   * Throw if profile file not found.
   */
  void updateProfile(const std::string & profileFilename, const FocusFinderProfileT & modifiedProfile);


  /**
   * Return a list of available profile (filenames).
   * In case there are no profiles, the vector is empty.
   */
  std::vector<std::string> getProfileFilenames() const;


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
