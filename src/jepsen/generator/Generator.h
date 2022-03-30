#ifndef GENERATOR_H
#define GENERATOR_H

#include "../include/log4cplus.h"
#include "Context.h"
#include <functional>
#include <list>
#include <memory>
#include <vector>

namespace jepsen {
namespace generator {

class Generator;
class OperationGenerator;
class FunctionGenerator;
class ListGenerator;
class Validate;
class OnThreads;
class TimeLimit;
class Stagger;

using GeneratorPtr = std::shared_ptr<Generator>;
using OperationGeneratorPtr = std::shared_ptr<OperationGenerator>;
using FunctionGeneratorPtr = std::shared_ptr<FunctionGenerator>;
using ListGeneratorPtr = std::shared_ptr<ListGenerator>;
using ValidatePtr = std::shared_ptr<Validate>;
using OnThreadsPtr = std::shared_ptr<OnThreads>;
using TimeLimitPtr = std::shared_ptr<TimeLimit>;
using StaggerPtr = std::shared_ptr<Stagger>;

// Functional Generator of 'op' and 'update'
std::pair<Operation, GeneratorPtr> op(GeneratorPtr gen, Context context);
GeneratorPtr update(GeneratorPtr gen, Context context, Operation event);

// Constant
// TODO: what is nil?
const std::pair<Operation, GeneratorPtr> nil = std::make_pair(Operation(Operation::kNil), nullptr);
bool notNil(Operation& op, GeneratorPtr& gen);
// Helper functions
Operation fillInOperation(const Context& context, Operation op);
// 根据filter，将不需要的thread从ctx的:free-threads中去除（留下满足filter的）
Context onThreadsContext(std::function<bool(int)> filter, Context ctx);
GeneratorPtr clients(GeneratorPtr client_gen);
GeneratorPtr nemesis(GeneratorPtr nemesis_gen);

// Generator Classs
class Generator {
public:
    Generator() = default;
    virtual std::pair<Operation, GeneratorPtr> op(Context context);
    virtual GeneratorPtr update(Context context, Operation event);
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

    explicit FunctionGenerator(std::function<Operation(Context)> func_with_context)
        : func_with_context(std::move(func_with_context)), func_type(FuncType::kWithCtx) {}

    explicit FunctionGenerator(std::function<Operation()> func_base)
        : func_base(std::move(func_base)), func_type(FuncType::kBase) {}

    GeneratorPtr copyOne() override;
    Operation func(Context context) const;

    std::function<Operation(Context)> func_with_context;
    std::function<Operation()> func_base;
    FuncType func_type;
};

class ListGenerator : public Generator {
public:
    friend std::pair<Operation, GeneratorPtr> generator::op(GeneratorPtr gen, Context context);
    friend GeneratorPtr generator::update(GeneratorPtr gen, Context context, Operation event);
    explicit ListGenerator(std::list<GeneratorPtr> generators)
        : generators(std::move(generators)) {}
    GeneratorPtr copyOne() override;

private:
    std::list<GeneratorPtr> generators;
};

class Validate : public Generator {
public:
    explicit Validate(const GeneratorPtr& gen)
        : gen(gen), logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("runner"))) {}

    std::pair<Operation, GeneratorPtr> op(Context context) override;
    GeneratorPtr update(Context context, Operation event) override;
    GeneratorPtr copyOne() override;

private:
    GeneratorPtr gen;
    log4cplus::Logger logger;
};


class FriendlyExceptions : public Generator {
    // TODO: Whether to introduce exception
};

class Trace : public Generator {
    /*
     * TODO: Wraps a generator, logging calls to op and update before passing them on to
     * the underlying generator. Takes a key k, which is included in every log line.
     */
};

class Map : public Generator {
    /*
     * TODO:A generator which wraps another generator g, transforming operations it
     * generates with (f op). When the underlying generator yields :pending or nil,
     * this generator does too, without calling `f`. Passes updates to underlying
     * generator.
     */
};

class Filter : public Generator {};

class IgnoreUpdates : public Generator {};


class OnThreads : public Generator {
public:
    OnThreads(const GeneratorPtr& gen, const std::function<bool(int)>& filter)
        : gen(gen), filter(filter) {}

    std::pair<Operation, GeneratorPtr> op(Context context) override;
    GeneratorPtr update(Context context, Operation event) override;
    GeneratorPtr copyOne() override;

private:
    GeneratorPtr gen;
    std::function<bool(int)> filter;
};

class Any : public Generator {};

class EachThread : public Generator {};

class Reserve : public Generator {};

class TimeLimit : public Generator {
public:
    TimeLimit(long limit, const GeneratorPtr& gen) : limit(limit), cutoff(kInvalidTime), gen(gen) {}
    TimeLimit(long limit, long cutoff, const GeneratorPtr& gen)
        : limit(limit), cutoff(cutoff), gen(gen) {}
    std::pair<Operation, GeneratorPtr> op(Context context) override;
    GeneratorPtr update(Context context, Operation event) override;
    GeneratorPtr copyOne() override;

private:
    long limit;
    long cutoff;
    GeneratorPtr gen;
};

class Stagger : public Generator {
public:
    Stagger(long dt, long nextTime, const GeneratorPtr& gen)
        : dt(dt), next_time(kInvalidTime), gen(gen) {}
    Stagger(long dt, const GeneratorPtr& gen) : dt(dt), gen(gen) {}
    std::pair<Operation, GeneratorPtr> op(Context context) override;
    GeneratorPtr update(Context context, Operation event) override;
    GeneratorPtr copyOne() override;

private:
    long dt;
    long next_time;
    GeneratorPtr gen;
};

}  // namespace generator
}  // namespace jepsen

#endif