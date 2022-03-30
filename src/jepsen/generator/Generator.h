#ifndef GENERATOR_H
#define GENERATOR_H

#include "Context.h"
#include <functional>
#include <memory>
#include <list>
#include <vector>

namespace jepsen {
namespace generator {

class Generator;
class OperationGenerator;
class FunctionGenerator;
class ListGenerator;
class Validate;
class OnThreads;

using GeneratorPtr = std::shared_ptr<Generator>;
using OperationGeneratorPtr = std::shared_ptr<OperationGenerator>;
using FunctionGeneratorPtr = std::shared_ptr<FunctionGenerator>;
using ListGeneratorPtr = std::shared_ptr<ListGenerator>;
using ValidatePtr = std::shared_ptr<Validate>;
using OnThreadsPtr = std::shared_ptr<OnThreads>;

Operation fillInOperation(const JepsenContext& context, Operation op);
std::pair<Operation, GeneratorPtr> op(GeneratorPtr gen, JepsenContext context);
GeneratorPtr update(GeneratorPtr gen, JepsenContext context, Operation event);

class Generator {
public:
    Generator() = default;
    virtual std::pair<Operation, GeneratorPtr> op(JepsenContext context);
    virtual GeneratorPtr update(JepsenContext context, Operation event);
    // TODO: 返回一个新的GeneratorPtr，内容与当前完全一致（注意是拷贝）
    virtual GeneratorPtr copyOne();
};

class OperationGenerator : public Generator {
public:
    explicit OperationGenerator(Operation operation) : operation(operation) {}
    GeneratorPtr copyOne() override;

    Operation operation;
};

class FunctionGenerator : public Generator {
public:
    enum FuncType { kWithCtx, kBase };

    explicit FunctionGenerator(std::function<Operation(JepsenContext)> func_with_context)
        : func_with_context(func_with_context), func_type(FuncType::kWithCtx) {}

    explicit FunctionGenerator(std::function<Operation()> func_base)
        : func_base(func_base), func_type(FuncType::kBase) {}

    GeneratorPtr copyOne() override;
    Operation func(JepsenContext context);

    std::function<Operation(JepsenContext)> func_with_context;
    std::function<Operation()> func_base;
    FuncType func_type;
};

class ListGenerator : public Generator {
public:
    explicit ListGenerator(std::list<GeneratorPtr> generators) : generators(std::move(generators)) {}
    GeneratorPtr copyOne() override;

    std::list<GeneratorPtr> generators;
};


}  // namespace generator
}  // namespace jepsen

#endif