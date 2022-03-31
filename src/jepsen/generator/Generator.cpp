#include "Generator.h"
#include "GeneratorFactory.h"

namespace jepsen {
namespace generator {

bool notNil(Operation& op, GeneratorPtr& gen) {
    return op.type != Operation::kNil && gen.get() != nullptr;
}


Operation fillInOperation(const Context& context, Operation op) {
    int process = context.randomFreeProcess();
    if (process != kInvalidProcess) {
        if (op.process == kInvalidProcess) {
            op.process = process;
        }
        if (op.time == 0) {
            op.time = context.time;
        }
        if (op.type == Operation::kInit) {
            op.type = Operation::kInvoke;
        }
    } else {
        op.type = Operation::kPending;
    }
    return op;
}

std::pair<Operation, GeneratorPtr> op(GeneratorPtr gen, Context context) {
    if (gen.get() == nullptr) {
        return nil;
    } else if (std::dynamic_pointer_cast<OperationGenerator>(gen) != nullptr) {
        OperationGeneratorPtr op_gen = std::dynamic_pointer_cast<OperationGenerator>(gen);
        auto ret_op = fillInOperation(context, op_gen->operation);
        if (ret_op.type == Operation::kPending) {
            return std::make_pair(ret_op, GeneratorFactory::createGenerator(op_gen->operation));
        } else {
            return std::make_pair(ret_op, nullptr);
        }
    } else if (std::dynamic_pointer_cast<FunctionGenerator>(gen) != nullptr) {
        FunctionGeneratorPtr fun_gen = std::dynamic_pointer_cast<FunctionGenerator>(gen);
        Operation ret_op = fun_gen->func(context);
        auto op_gen = GeneratorFactory::createGenerator(ret_op);
        ListGeneratorPtr list_gen = GeneratorFactory::createGenerator({op_gen, gen->copyOne()});
        return op(list_gen, context);
    } else if (std::dynamic_pointer_cast<ListGenerator>(gen) != nullptr) {
        ListGeneratorPtr list_gen = std::dynamic_pointer_cast<ListGenerator>(gen);
        if (list_gen->generators.empty()) {
            return op(GeneratorFactory::createNullptrGenerator(), context);
        } else {
            auto first_gen = list_gen->generators.front();
            auto [op2, gen2] = op(first_gen, context);
            ListGeneratorPtr copy = std::dynamic_pointer_cast<ListGenerator>(gen->copyOne());
            if (gen2.get() == nullptr && op2.type == Operation::kNil) {
                copy->generators.pop_front();
                return op(copy, context);
            } else {
                if (copy->generators.size() > 1) {
                    copy->generators.front() = gen2;
                    return std::make_pair(op2, copy);
                } else {
                    return std::make_pair(op2, gen2);
                }
            }
        }
    } else {
        return gen->op(context);
    }
}

GeneratorPtr update(GeneratorPtr gen, Context context, Operation event) {
    if (gen.get() == nullptr) {
        return nullptr;
    } else if (std::dynamic_pointer_cast<OperationGenerator>(gen) != nullptr) {
        return gen->copyOne();
    } else if (std::dynamic_pointer_cast<FunctionGenerator>(gen) != nullptr) {
        return gen->copyOne();
    } else if (std::dynamic_pointer_cast<ListGenerator>(gen) != nullptr) {
        ListGeneratorPtr list_gen = std::dynamic_pointer_cast<ListGenerator>(gen);
        if (list_gen->generators.empty()) {
            return update(GeneratorFactory::createNullptrGenerator(), context, event);
        } else {
            GeneratorPtr upd = update(list_gen->generators.front(), context, event);
            ListGeneratorPtr copy = std::dynamic_pointer_cast<ListGenerator>(gen->copyOne());
            copy->generators.front() = upd;
            return copy;
        }
    } else {
        return gen->update(context, event);
    }
}

Context onThreadsContext(std::function<bool(int)> filter, Context ctx) {
    Context ret = ctx;
    ret.free_threads.clear();
    for (auto t : ctx.free_threads) {
        if (filter(t)) {
            ret.free_threads.insert(t);
        }
    }
    return ret;
}

GeneratorPtr clients(GeneratorPtr client_gen) {
    return GeneratorFactory::createOnThreads(client_gen,
                                             [](int id) { return id != kNemesisProcess; });
}

GeneratorPtr nemesis(GeneratorPtr nemesis_gen) {
    return GeneratorFactory::createOnThreads(nemesis_gen,
                                             [](int id) { return id == kNemesisProcess; });
}

GeneratorPtr timeLimit(long dt, GeneratorPtr gen) {
    return GeneratorFactory::createTimeLimit(sec2nanos(dt), gen);
}

GeneratorPtr stagger(long dt, GeneratorPtr gen) {
    return GeneratorFactory::createStagger(2 * sec2nanos(dt), gen);
}

GeneratorPtr mix(std::vector<GeneratorPtr> gens) {
    int idx = rand() % gens.size();
    return GeneratorFactory::createMix(idx, gens);
}

GeneratorPtr mix(std::vector<Operation> ops) {
    std::vector<GeneratorPtr> gens;
    gens.reserve(ops.size());
    for (auto& op : ops) {
        gens.push_back(GeneratorFactory::createGenerator(op));
    }
    return mix(gens);
}

std::pair<Operation, GeneratorPtr> generator::Generator::op(Context context) {
    return std::make_pair(Operation(Operation::kNil), nullptr);
}

GeneratorPtr generator::Generator::update(Context context, Operation event) {
    return nullptr;
}

GeneratorPtr Generator::copyOne() {
    return nullptr;
}

GeneratorPtr OperationGenerator::copyOne() {
    return std::make_shared<OperationGenerator>(operation);
}
GeneratorPtr FunctionGenerator::copyOne() {
    switch (func_type) {
        case kWithCtx:
            return std::make_shared<FunctionGenerator>(func_with_context);
        case kBase:
            return std::make_shared<FunctionGenerator>(func_base);
        default:
            return nullptr;
    }
}
Operation FunctionGenerator::func(Context context) const {
    Operation ret_op = func_type == FuncType::kBase ? func_base() : func_with_context(context);
    return ret_op;
}

GeneratorPtr ListGenerator::copyOne() {
    std::list<GeneratorPtr> copy;
    for (auto& gen : generators) {
        if (gen) {
            copy.push_back(gen->copyOne());
        } else {
            copy.push_back(nullptr);
        }
    }
    return std::make_shared<ListGenerator>(copy);
}


std::pair<Operation, GeneratorPtr> Validate::op(Context context) {
    auto [op2, gen2] = generator::op(gen, context);
    if (gen2.get() != nullptr && op2.type != Operation::kNil) {
        if (op2.type == Operation::kPending) {
            // Do nothing
        } else {
            std::vector<std::string> problems;
            // TODO: :invoke, :info, :sleep, :log
            if (op2.type != Operation::kInvoke) {
                problems.emplace_back(":type should be :invoke, :info, :sleep, or :log");
            }
            if (!problems.empty()) {
                for (auto& problem : problems) {
                    LOG4CPLUS_ERROR(logger, problem.c_str());
                }
            }
        }
        return std::make_pair(op2, GeneratorFactory::createValidate(gen2));
    } else {
        return std::make_pair(op2, gen2);
    }
}
GeneratorPtr Validate::update(Context context, Operation event) {
    GeneratorPtr gen2 = generator::update(gen, context, event);
    return GeneratorFactory::createValidate(gen2);
}
GeneratorPtr Validate::copyOne() {
    if (gen.get() == nullptr) {
        return GeneratorFactory::createValidate(nullptr);
    } else {
        auto gen2 = gen->copyOne();
        return GeneratorFactory::createValidate(gen2);
    }
}

std::pair<Operation, GeneratorPtr> OnThreads::op(Context context) {
    auto [op2, gen2] = generator::op(gen, generator::onThreadsContext(filter, context));
    if (gen2.get() != nullptr && op2.type != Operation::kNil) {
        return std::make_pair(op2, GeneratorFactory::createOnThreads(gen2, filter));
    } else {
        return nil;
    }
}
GeneratorPtr OnThreads::update(Context context, Operation event) {
    return Generator::update(context, event);
}
GeneratorPtr OnThreads::copyOne() {
    if (gen.get() == nullptr) {
        return GeneratorFactory::createOnThreads(nullptr, filter);
    } else {
        return GeneratorFactory::createOnThreads(gen->copyOne(), filter);
    }
}


std::pair<Operation, GeneratorPtr> TimeLimit::op(Context context) {
    auto [op2, gen2] = generator::op(gen, context);
    if (notNil(op2, gen2)) {
        switch (op2.type) {
            case Operation::kPending:
                return std::make_pair(op2, std::make_shared<TimeLimit>(limit, cutoff, gen2));
            default:
                if (cutoff == kInvalidTime) {
                    cutoff = op2.time + limit;
                }
                if (op2.time < cutoff) {
                    return std::make_pair(op2, std::make_shared<TimeLimit>(limit, cutoff, gen2));
                } else {
                    return nil;
                }
        }
    } else {
        return nil;
    }
}
GeneratorPtr TimeLimit::update(Context context, Operation event) {
    auto gen2 = generator::update(gen, context, event);
    return GeneratorFactory::createTimeLimit(limit, cutoff, gen2);
}
GeneratorPtr TimeLimit::copyOne() {
    if (gen.get() == nullptr) {
        return nullptr;
    } else {
        return GeneratorFactory::createTimeLimit(limit, cutoff, gen->copyOne());
    }
}

std::pair<Operation, GeneratorPtr> Stagger::op(Context context) {
    auto [op2, gen2] = generator::op(gen, context);
    if (notNil(op2, gen2)) {
        if (next_time == kInvalidTime) {
            next_time = context.time;
        }
        if (op2.type == Operation::kPending) {
            return std::make_pair(op2, std::make_shared<Stagger>(*this));
        } else {
            auto next_gen = GeneratorFactory::createStagger(dt, next_time + rand() % dt, gen2);
            if (next_time > op2.time) {
                op2.time = next_time;
            }
            return std::make_pair(op2, next_gen);
        }
    } else {
        return nil;
    }
}
GeneratorPtr Stagger::update(Context context, Operation event) {
    auto gen2 = generator::update(gen, context, event);
    return GeneratorFactory::createStagger(dt, next_time, gen2);
}
GeneratorPtr Stagger::copyOne() {
    if (gen.get() == nullptr) {
        return nullptr;
    } else {
        return GeneratorFactory::createStagger(dt, next_time, gen->copyOne());
    }
}

std::pair<Operation, GeneratorPtr> Mix::op(Context context) {
    if (!gens.empty()) {
        auto [op2, gen2] = generator::op(gens[idx], context);
        std::vector<GeneratorPtr> copy;
        copy.reserve(gens.size());

        if (notNil(op2, gen2)) {
            for(auto& gen: gens) {
                copy.push_back(gen->copyOne());
            }
            copy[idx] = gen2;
            return std::make_pair(op2, mix(copy));
        } else {
            for(int i=0; i<gens.size(); i++) {
                if(i == idx) {
                    continue;
                }
                copy.push_back(gens[i]->copyOne());
            }
            return generator::op(mix(copy), context);
        }
    }
    return nil;
}
GeneratorPtr Mix::update(Context context, Operation event) {
    return GeneratorFactory::createMix(idx, gens);
}
GeneratorPtr Mix::copyOne() {
    std::vector<GeneratorPtr> copy;
    copy.reserve(gens.size());
    for(auto& gen: gens) {
        copy.push_back(gen->copyOne());
    }
    return GeneratorFactory::createMix(idx, gens);;
}

}  // namespace generator
}  // namespace jepsen