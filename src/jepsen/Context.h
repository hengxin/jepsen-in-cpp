#ifndef CONTEXT_H
#define CONTEXT_H

#include "Worker.h"
#include <set>
#include <unordered_map>

namespace jepsen {

class JepsenContext {
public:
    JepsenContext(int concurrency) : time(0) {
        for (int i = 0; i <= concurrency; i++) {
            free_threads.insert(i);
        }
    }

private:
    int time;
    std::set<int> free_threads;
};

}  // namespace jepsen


#endif