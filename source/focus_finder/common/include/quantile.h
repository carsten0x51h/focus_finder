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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_QUANTILE_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_QUANTILE_H_

#include <algorithm>    // std::copy, std::sort, std::max, std::min
#include <vector>
#include <cinttypes>	// int64_t
#include <cmath>		// std::floor, std::ceil, std::lerp

class QuantileT {

public:
	/**
	 * Based on proposal from https://stackoverflow.com/questions/11964552/finding-quartiles
	 */
	template<typename T, typename RngT>
	static T calc(const RngT & inData, T prob)
	{
	  if (inData.empty()) { return 0; } // TODO: Is returning 0 ok?
	  if (1 == inData.size()) { return *inData.begin(); }

	  std::vector<T> data;
	  std::copy(inData.begin(), inData.end(), std::back_inserter(data));
	  std::sort(data.begin(), data.end());

	  // TODO: Remove... lerp now part of std
	  //T poi = Lerp<T>(-0.5, data.size() - 0.5, prob);
	  T poi = std::lerp(-0.5f, data.size() - 0.5f, prob);

	  size_t left = std::max(int64_t(std::floor(poi)), int64_t(0));
	  size_t right = std::min(int64_t(std::ceil(poi)), int64_t(data.size() - 1));

	  T datLeft = data.at(left);
	  T datRight = data.at(right);

	  // Linear interpolation between two values
	  // https://en.cppreference.com/w/cpp/numeric/lerp
	  //T q = lerp<T>(datLeft, datRight, poi - left); // TODO: Remove... now supported in C++-20! :)
	  return std::lerp(datLeft, datRight, poi - left);
	}
};

#endif /*SOURCE_FOCUS_FINDER_COMMON_INCLUDE_QUANTILE_H_*/
