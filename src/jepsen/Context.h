#ifndef CONTEXT_H
#define CONTEXT_H

#include "Worker.h"
#include <chrono>
#include <random>
#include <set>
#include <unordered_map>

namespace jepsen {

class Context {
public:
    Context(int concurrency) : time(std::chrono::system_clock::now().time_since_epoch().count()) {
        for (int i = 0; i < concurrency; i++) {
            free_threads.insert(i);
        }
        free_threads.insert(kNemesisProcess);
    }

    int randomFreeProcess() const;
    int nextProcess(int thread);
    int processToThread(int process);
    long long time;
    std::set<int> free_threads;
    std::unordered_map<int, int> workers;  // thread->process
};

}  // namespace jepsen


#endif