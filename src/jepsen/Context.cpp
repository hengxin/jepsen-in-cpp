#include "Context.h"

namespace jepsen {

int Context::randomFreeProcess() const {
    int n = free_threads.size();
    if (n != 0) {
        auto it = free_threads.begin();
        // non-deterministic generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, n - 1);
        std::advance(it, dist(gen));
        int thread = *it;
        return workers.at(thread);
    } else {
        return kInvalidProcess;
    }
}

int Context::nextProcess(int thread) {
    if (thread != kNemesisProcess) {
        int cnt = std::count_if(
            workers.begin(), workers.end(), [](auto tp) { return tp.second != kNemesisProcess; });
        return workers[thread] + cnt;
    } else {
        return thread;
    }
}

int Context::processToThread(int process) {
    for (const auto& [t, p] : workers) {
        if (p == process) {
            return t;
        }
    }
    return kInvalidProcess;
}

}  // namespace jepsen