/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c [at] lost-infinity.com>
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

#include "include/fofi_project_info.h"

/**
 * Version is set in <root>/CMakeLists.txt - e.g.:
 *
 * set(META_VERSION_MAJOR       "1")
 * set(META_VERSION_MINOR       "0")
 * set(META_VERSION_PATCH       "0")
 */
#include "fofi/fofi-version.h"

std::string FoFiProjectInfoT::getProjectName() {
    return std::string(FOFI_PROJECT_NAME);
}

std::string FoFiProjectInfoT::getProjectDescription() {
    return std::string(FOFI_PROJECT_DESCRIPTION);
}

std::string FoFiProjectInfoT::getOrganization() {
    return std::string(FOFI_AUTHOR_ORGANIZATION);
}

std::string FoFiProjectInfoT::getWebsite() {
    return std::string(FOFI_AUTHOR_DOMAIN);
}

std::string FoFiProjectInfoT::getMailAddress() {
    return std::string(FOFI_AUTHOR_MAINTAINER);
}

int FoFiProjectInfoT::getVersionMajor() {
    return std::stoi(FOFI_VERSION_MAJOR);
}

int FoFiProjectInfoT::getVersionMinor() {
    return std::stoi(FOFI_VERSION_MINOR);
}

int FoFiProjectInfoT::getVersionPatch() {
    return std::stoi(FOFI_VERSION_PATCH);
}

std::string FoFiProjectInfoT::getVersionRevision() {
    return std::string(FOFI_VERSION_REVISION);
}

std::string FoFiProjectInfoT::getVersionString() {
    return std::string(FOFI_VERSION);
}

std::string FoFiProjectInfoT::getNamedVersion() {
    return std::string(FOFI_NAME_VERSION);
}
