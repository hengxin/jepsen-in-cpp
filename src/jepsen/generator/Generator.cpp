#include "Generator.h"
#include "GeneratorFactory.h"

namespace jepsen {
namespace generator {

Operation fillInOperation(const JepsenContext& context, Operation op) {
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

std::pair<Operation, GeneratorPtr> op(GeneratorPtr gen, JepsenContext context) {
    if (gen.get() == nullptr) {
        return {Operation(Operation::kNil), nullptr};
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
            auto[op2, gen2] = op(first_gen, context);
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

GeneratorPtr update(GeneratorPtr gen, JepsenContext context, Operation event) {
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

std::pair<Operation, GeneratorPtr> generator::Generator::op(JepsenContext context) {
    return std::make_pair(Operation(Operation::kNil), nullptr);
}

GeneratorPtr generator::Generator::update(JepsenContext context, Operation event) {
    return jepsen::generator::GeneratorPtr();
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
Operation FunctionGenerator::func(JepsenContext context) {
    Operation ret_op = func_type == FuncType::kBase ? func_base() : func_with_context(context);
    return ret_op;
}

GeneratorPtr ListGenerator::copyOne() {
    std::list<GeneratorPtr> copy;
    for (auto &gen: generators) {
        if (gen) {
            copy.push_back(gen->copyOne());
        } else {
            copy.push_back(nullptr);
        }
    }
    return std::make_shared<ListGenerator>(copy);
}

}  // namespace generator
}  // namespace jepsen