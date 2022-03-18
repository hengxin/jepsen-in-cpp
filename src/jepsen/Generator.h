#ifndef GENERATOR_H
#define GENERATOR_H

#include "Context.h"
#include <memory>
#include <queue>
#include <vector>

namespace jepsen {
class Generator;
using GeneratorPtr = std::shared_ptr<Generator>;

class Generator {
public:
    Generator() = default;
    virtual void update(const JepsenContext& context, const Operation& op) = 0;
    virtual Operation op(const JepsenContext& context) = 0;
};

class VectorGenerator : public Generator {
public:
    VectorGenerator(const std::vector<Operation>& opvecs) {
        for (auto& op : opvecs) {
            operations.emplace(op);
        }
    }
    virtual void update(const JepsenContext& context, const Operation& op) {}
    virtual Operation op(const JepsenContext& context) {
        auto& op = operations.front();
        operations.pop();
        return op;
    }

private:
    std::queue<Operation> operations;
};

}  // namespace jepsen

#endif