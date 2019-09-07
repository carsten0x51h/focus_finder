#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_HOME_DIRECTORY_FINDER_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_HOME_DIRECTORY_FINDER_H_

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string>

// TODO: Introduce #ifdef depending on platform. So far we only support the Unix platform.
class HomeDirectoryFinderT {
 public:
	/**
	 * Unix implementation.
	 */
	static std::string getHomeDir() {
    // See https://stackoverflow.com/questions/2910377/get-home-directory-in-linux
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    return std::string(homedir);
  }
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_HOME_DIRECTORY_FINDER_H_*/
