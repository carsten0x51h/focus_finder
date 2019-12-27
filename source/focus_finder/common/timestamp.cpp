#include <ostream>
#include <chrono>
#include <ratio>
#include "include/timestamp.h"

std::ostream & operator<<(std::ostream & os, const TimestampT & ts) {
  os << std::chrono::duration_cast<std::chrono::milliseconds>(ts.time_since_epoch()).count();
  return os;
}
