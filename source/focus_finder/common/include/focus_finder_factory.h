/*
 * cooler.h
 *
 *  Created on: Apr 12, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_FACTORY_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_FACTORY_H_

#include <memory>

#include "focus_finder_strategy.h"
#include "enum_helper.h"

class FocusFinderT;

class FocusFinderFactoryT {
public:
	static std::shared_ptr<FocusFinderT> getInstance(const FocusFinderStrategyT::TypeE & strategy);
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_FOCUS_FINDER_FACTORY_H_ */
