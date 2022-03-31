//
// Created by young on 2022/3/31.
//

#ifndef JEPSEN_IN_CPP_TIMEHELPER_H
#define JEPSEN_IN_CPP_TIMEHELPER_H

#include <chrono>

namespace jepsen {


long getRelativeTime();
long sec2nanos(long seconds);

}

#endif  // JEPSEN_IN_CPP_TIMEHELPER_H
