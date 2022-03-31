//
// Created by Young on 2022/3/28.
//

#ifndef JEPSEN_IN_CPP_GENERATORFACTORY_H
#define JEPSEN_IN_CPP_GENERATORFACTORY_H

#include "Generator.h"

namespace jepsen {
namespace generator {

class GeneratorFactory {
public:
    static GeneratorPtr createNullptrGenerator() {
        return nullptr;
    }

    static OperationGeneratorPtr createGenerator(Operation op) {
        return std::make_shared<OperationGenerator>(OperationGenerator(op));
    }

    static FunctionGeneratorPtr createGenerator(
        const std::function<Operation(Context)> func_with_context) {
        return std::make_shared<FunctionGenerator>(FunctionGenerator(func_with_context));
    }

    static FunctionGeneratorPtr createGenerator(const std::function<Operation()>& func_base) {
        return std::make_shared<FunctionGenerator>(FunctionGenerator(std::move(func_base)));
    }

    static ListGeneratorPtr createGenerator(const std::list<Operation>& operations) {
        std::list<GeneratorPtr> generators;
        for (auto& op : operations) {
            generators.push_back(createGenerator(op));
        }
        return std::make_shared<ListGenerator>(generators);
    }

    static ListGeneratorPtr createGenerator(const std::list<GeneratorPtr>& generators) {
        return std::make_shared<ListGenerator>(ListGenerator(generators));
    }

    static ValidatePtr createValidate(GeneratorPtr gen) {
        return std::make_shared<Validate>(gen);
    }

    static OnThreadsPtr createOnThreads(GeneratorPtr gen, std::function<bool(int)> filter) {
        return std::make_shared<OnThreads>(gen, filter);
    }

    static TimeLimitPtr createTimeLimit(long dt, GeneratorPtr gen) {
        return std::make_shared<TimeLimit>(dt, gen);
    }

    static TimeLimitPtr createTimeLimit(long dt, long cutoff, GeneratorPtr gen) {
        return std::make_shared<TimeLimit>(dt, cutoff, gen);
    }

    static StaggerPtr createStagger(long dt, GeneratorPtr gen) {
        return std::make_shared<Stagger>(dt, gen);
    }

    static StaggerPtr createStagger(long dt, long next_time, GeneratorPtr gen) {
        return std::make_shared<Stagger>(dt, next_time, gen);
    }

    static MixPtr createMix(int idx, std::vector<GeneratorPtr> gens) {
        return std::make_shared<Mix>(idx, gens);
    }

};
}  // namespace generator
}  // namespace jepsen


#endif  // JEPSEN_IN_CPP_GENERATORFACTORY_H
