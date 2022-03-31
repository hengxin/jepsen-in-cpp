//
// Created by young on 2022/3/31.
//

#include "TimeHelper.h"

namespace jepsen {

long getRelativeTime(){
    return std::chrono::system_clock::now().time_since_epoch().count();
}

long sec2nanos(long seconds) {
    return seconds * 1e9;
}

}