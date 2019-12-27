#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TIMESTAMP_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TIMESTAMP_H_

#include <chrono>

typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimestampT;
std::ostream & operator<<(std::ostream & os, const TimestampT & ts);


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_TIMESTAMP_H_ */
