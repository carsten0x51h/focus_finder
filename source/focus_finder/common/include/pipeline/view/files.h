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

#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/iterator_range.hpp>

#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

/**
 * Return the filenames of all files that have the specified extension
 * in the specified directory and all subdirectories.
 *
 * See https://stackoverflow.com/questions/11140483/how-to-get-list-of-files-with-a-specific-extension-in-a-given-folder
 */
namespace starmath::pipeline {

        // TODO / FIXME / HACK! Template is actually not needed here...
        //                      it only helps to remove "multiple definition" errors..,
        //     ranges::any_view<int> ???

        template<typename MyType=std::string>
        auto
        files(const std::string &extensionRegex = "") {
            return ranges::views::transform(
                [=](const std::string &rootPath) {
                    const std::regex e(extensionRegex);
                    const fs::path root(rootPath);

                    // See https://en.cppreference.com/w/cpp/filesystem/directory_iterator
                    // ... These specializations for directory_iterator make it a borrowed_range and a view.
                    //
                    // NOTE: https://github.com/ericniebler/range-v3/issues/1400
                    // See https://godbolt.org/z/-Vu-Md.
                    // std::filesystem::iterator is not a std::safe_range, so you must
                    // first make it an object to prevent dangling.
                    // However, ranges::make_iterator_range() works for now, but is it a good idea?

                    return ranges::make_iterator_range(begin(fs::directory_iterator(root)),
                                                       end(fs::directory_iterator(root)))
                           | ranges::views::filter([](const auto &entry) { return fs::is_regular_file(entry); })
                           | ranges::views::filter([=](const auto &entry) {
                                return (extensionRegex.empty() || std::regex_match(entry.path().string(), e));
                            })
                           | ranges::views::transform([](const auto &entry) { return entry.path().string(); });
                }
            );
        }
    }

#endif //FOFI_FILES_H
