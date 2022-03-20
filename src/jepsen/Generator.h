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
    void fillIn(const JepsenContext& context, Operation& op);
};

class VectorGenerator : public Generator {
public:
    VectorGenerator(const std::vector<Operation>& opvecs) {
        for (auto& op : opvecs) {
            operations.emplace(op);
        }
    }
    virtual void update(const JepsenContext& context, const Operation& op) override {}
    virtual Operation op(const JepsenContext& context) override {
        if (!operations.empty()) {
            auto op = operations.front();
            operations.pop();
            fillIn(context, op);
            return op;
        } else {
            return OperationFactory::exit();
        }
    }

private:
    std::queue<Operation> operations;
};

}  // namespace jepsen

#endif