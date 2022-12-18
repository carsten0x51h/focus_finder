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

#ifndef FOFI_FILES_H
#define FOFI_FILES_H

#include <range/v3/all.hpp>

#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

/**
 * Return the filenames of all files that have the specified extension
 * in the specified directory and all subdirectories.
 *
 * See https://stackoverflow.com/questions/11140483/how-to-get-list-of-files-with-a-specific-extension-in-a-given-folder
 */
namespace AstroImagePipeline {

    auto
    files(const std::string & extensionRegex = "") {
        return ranges::views::transform(
            [=](const std::string & rootPath) {
                std::vector<std::string> paths;
                const std::regex e(extensionRegex);
                const fs::path root(rootPath);

                if (fs::exists(root) && fs::is_directory(root)) {
                    for (auto const & entry : fs::directory_iterator(root)) {
                        if (fs::is_regular_file(entry) && (extensionRegex.empty() || std::regex_match(entry.path().string(), e))) {
                            paths.push_back(entry.path().string());
                        }
                    }
                }
                return paths;
            }
        );
    }

} // End namespace AstroImagePipeline

#endif //FOFI_FILES_H
