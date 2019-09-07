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
