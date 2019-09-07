#include <memory>
#include <algorithm>
#include <vector>
#include <filesystem>

#include <boost/algorithm/string/predicate.hpp>

#include "include/logging.h"
#include "include/profile_manager.h"
#include "include/focus_finder_profile.h"
#include "include/home_directory_finder.h"

std::string ProfileManagerT::composeFullPath(
		const std::string & profileFileName) const {
	// TODO: Use filesystem path to construct sDefaultPath..
	return mProfileRootPath + "/" + profileFileName;
}

ProfileManagerT::ProfileManagerT() :
		mActiveProfile(std::nullopt), mProfileRootPath(
				HomeDirectoryFinderT::getHomeDir() + "/.fofi") // TODO: Use filesystem path to construct sDefaultPath..
{
	// Create ".fofi" in home directory if it does not exist.
	// TODO: Only create if it does not exist...
	// TODO: Error handling...
	std::filesystem::create_directories(mProfileRootPath);
}

ProfileManagerT::~ProfileManagerT() {
}

std::optional<FocusFinderProfileT> ProfileManagerT::getActiveProfile() const {
	return mActiveProfile;
}

std::string ProfileManagerT::getActiveProfileFilename() const {
	return mActiveProfileFilename;
}

void ProfileManagerT::clearActiveProfile() {
	mActiveProfile.reset();
	mActiveProfileFilename = ""; // TODO: nullptr? or null?

	// Notify all listeners about profile change
	// TODO: Only notify if something has changed?
	notifyActiveProfileChanged();
}

void ProfileManagerT::activateProfile(const std::string & profileFilename) {
	try {
		mActiveProfile.emplace(
				FocusFinderProfileT::load(composeFullPath(profileFilename)));
		mActiveProfileFilename = profileFilename;

		// Notify all listeners about profile change
		notifyActiveProfileChanged();

	} catch (FocusFinderProfileExceptionT & exc) {
		throw ProfileManagerExceptionT(exc.what());
	}
}

void ProfileManagerT::deleteProfile(const std::string & profileFilename) {
	if (profileFilename.empty()) {
		throw ProfileManagerExceptionT("No profile filename specified.");
	}

	std::error_code ec;
	std::string fullPathToprofileFile = composeFullPath(profileFilename);

	// Check if file exists
	bool exists = std::filesystem::exists(fullPathToprofileFile, ec);

	if (ec) {
		// Error even checking if file is there...
		throw ProfileManagerExceptionT(ec.message());
	}

	if (!exists) {
		throw ProfileManagerExceptionT("Profile file not found.");
	}

	// Delete profile from filesystem
	std::filesystem::remove(fullPathToprofileFile, ec);

	if (ec) {
		// Error deleting file...
		throw ProfileManagerExceptionT(ec.message());
	}

	// Notify that profile list has changed
	notifyProfileListChanged();

	// If the deleted one is the currently active profile, clear the active profile.
	if (mActiveProfileFilename == profileFilename) {
		LOG(debug)
		<< "ProfileManagerT::deleteProfile... mActiveProfileFilename = "
				<< mActiveProfileFilename << " == profileFilename = " << profileFilename
				<< " -> clearing active profile..." << std::endl;

		clearActiveProfile();
	}
}

void ProfileManagerT::addProfile(const std::string & profileFilename,
		const FocusFinderProfileT & newProfile) {
	if (profileFilename.empty()) {
		throw ProfileManagerExceptionT("No profile filename specified.");
	}

	// Check if file is there already (how to handle?)
	std::error_code ec;
	std::string fullPathToprofileFile = composeFullPath(profileFilename);

	// Check if file exists
	bool exists = std::filesystem::exists(fullPathToprofileFile, ec);

	if (ec) {
		// Error even checking if file is there...
		throw ProfileManagerExceptionT(ec.message());
	}

	if (exists) {
		throw ProfileManagerExceptionT("Profile file already exists.");
	}

	// Store to file
	try {
		FocusFinderProfileT::save(fullPathToprofileFile, newProfile);
	} catch (FocusFinderProfileExceptionT & exc) {
		throw ProfileManagerExceptionT(exc.what());
	}

	// Notify that profile list has changed
	notifyProfileListChanged();
}

void ProfileManagerT::updateProfile(const std::string & profileFilename,
		const FocusFinderProfileT & modifiedProfile) {
	if (profileFilename.empty()) {
		throw ProfileManagerExceptionT("No profile filename specified.");
	}

	std::string fullPathToprofileFile = composeFullPath(profileFilename);

	// Store to file
	try {
		FocusFinderProfileT::save(fullPathToprofileFile, modifiedProfile);
	} catch (FocusFinderProfileExceptionT & exc) {
		throw ProfileManagerExceptionT(exc.what());
	}

	// In case the updated profile is currently the active one, overwrite the active one with the modified one and notify listeners.
	if (mActiveProfileFilename == profileFilename) {
		// Set new active profile
		mActiveProfile.emplace(modifiedProfile);
		mActiveProfileFilename = profileFilename;

		// Notify all listeners about profile change
		notifyActiveProfileChanged();
	}
}

std::vector<std::string> ProfileManagerT::getProfileFilenames() const {
	std::vector<std::string> profileFilenames;

	for (auto& p : std::filesystem::directory_iterator(mProfileRootPath)) {
		auto filePath = p.path();
		bool isRegularFile = std::filesystem::is_regular_file(p.status());

		if (isRegularFile
				&& boost::iequals(filePath.extension().c_str(), ".fofi")) {
			profileFilenames.push_back(filePath.filename());
		}
	}
	return profileFilenames;
}
