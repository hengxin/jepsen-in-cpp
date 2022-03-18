#ifndef NEMESIS_H
#define NEMESIS_H

#include "Client.h"

namespace jepsen {
// Refer to https://www.zhihu.com/question/419982799/answer/1461745869
class Nemesis;
using NemesisPtr = unique_ptr<Nemesis>;
using NemesisCreator = std::function<NemesisPtr()>;

template <class T>
struct NemesisConstructorHelper {
    static_assert(std::is_base_of<Nemesis, T>::value, "T must derived from Nemesis!");
    NemesisPtr operator()() {
        return NemesisPtr(new T);
    }
};

class Nemesis : public Client {
public:
    virtual bool open(string node) = 0;
    virtual bool close() = 0;
    virtual bool setup() = 0;
    virtual bool invoke(Operation& op) = 0;
    virtual bool teardown() = 0;

    template <class T>
    static void Register() {
        cur_nemesis_type = NemesisConstructorHelper<T>();
    }

    static NemesisPtr createOne() {
        return cur_nemesis_type();
    }

public:
    static NemesisCreator cur_nemesis_type;
};

class NoopNemesis : public Nemesis {
public:
    NoopNemesis() {
        LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__);
    };
    virtual bool open(string node) override {
        return true;
    };
    virtual bool close() override {
        return true;
    };
    virtual bool setup() override {
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("NoopNemesis setup"));
        return true;
    };
    virtual bool invoke(Operation& op) override {
        return true;
    };
    virtual bool teardown() override {
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("NoopNemesis teardown"));
        return true;
    };
};
}  // namespace jepsen


#endif