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
        return *it;
    } else {
        return kInvalidProcess;
    }
}

}  // namespace jepsen